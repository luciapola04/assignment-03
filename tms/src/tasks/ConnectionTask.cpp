#include "tasks/ConnectionTask.h"
#include "kernel/Logger.h"

WiFiClient espClient;
PubSubClient client(espClient);

ConnectionTask::ConnectionTask(Context* pContext) {
    this->pContext = pContext;
    this->pContext->setMQTTClient(&client);
    setState(DISCONNECTED);
}

void ConnectionTask::tick() {
    switch (state) {
    

    case DISCONNECTED:
        if (checkAndSetJustEntered()) {
            client.disconnect();
            WiFi.disconnect();

            log("Stato: DISCONNECTED. Avvio WiFi...");
            WiFi.mode(WIFI_STA);
            WiFi.begin(ssid, password);
            pContext->setSystemOnline(false);
            pContext->setMqttError(false);
        }

        if (WiFi.status() == WL_CONNECTED) {
            log("WiFi Connesso! IP: " + WiFi.localIP().toString());
            setState(CONNECTING_MQTT);
        }
        else if (elapsedTimeInState() > 10000) {
            log("Timeout WiFi. Riprovo...");
            WiFi.disconnect();
            setState(DISCONNECTED);
        }
        break;

    case CONNECTING_MQTT:
        if (checkAndSetJustEntered()) {
            log("Stato: CONNECTING_MQTT. Setup Server...");
            client.setServer(mqtt_server, 1883);
            pContext->setSystemOnline(false);
            
        }

        if (!client.connected()) {
            if (elapsedTimeInState() > 2000) {
                String clientId = String("esiot-2025-client-") + String(random(0xffff), HEX);
                log("Tentativo connessione MQTT ID: " + clientId);
                
                if (client.connect(clientId.c_str())) {
                    log("MQTT Connesso!");
                    client.subscribe(mqtt_topic);
                    setState(CONNECTED);
                } else {
                    log("Fallito, rc=" + String(client.state()) + ". Riprovo...");
                    stateTimestamp = millis();
                }
            }
        } else {
            setState(CONNECTED);
        }
        
        if (WiFi.status() != WL_CONNECTED) {
            setState(DISCONNECTED);
        }
        break;

    case CONNECTED:
        if (checkAndSetJustEntered()) {
            log("Stato: CONNECTED (Sistema Online)");
            pContext->setSystemOnline(true);
        }

        client.loop();

        if (WiFi.status() != WL_CONNECTED) {
            log("Persa connessione WiFi!");
            setState(DISCONNECTED);
        } else if (!client.connected() || this->pContext->isMqttError()) {
            log("Persa connessione MQTT!");
            setState(DISCONNECTED);
        }
        break;
    }
}


void ConnectionTask::setState(ConnectionState newState) {
    this->state = newState;
    this->stateTimestamp = millis();
    this->justEntered = true;
}

long ConnectionTask::elapsedTimeInState() {
    return millis() - stateTimestamp;
}

bool ConnectionTask::checkAndSetJustEntered() {
    if (justEntered) {
        justEntered = false;
        return true;
    }
    return false;
}

void ConnectionTask::log(const String& msg) {
    Logger.log("[ConnTask] " + msg);
}