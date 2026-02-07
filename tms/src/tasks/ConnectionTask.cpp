#include "tasks/ConnectionTask.h"
#include "kernel/Logger.h"

WiFiClient espClient;
PubSubClient client(espClient);

ConnectionTask::ConnectionTask(Context* pContext) {
    this->pContext = pContext;
    setState(DISCONNECTED);
}

void ConnectionTask::tick() {
    switch (state) {
    

    case DISCONNECTED:
        if (checkAndSetJustEntered()) {
            log("Stato: DISCONNECTED. Avvio WiFi...");
            WiFi.mode(WIFI_STA);
            WiFi.begin(ssid, password);
        }
        log("prova");

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
        
        // Se si perde il WiFi mentre cerchiamo MQTT
        if (WiFi.status() != WL_CONNECTED) {
            setState(DISCONNECTED);
        }
        break;

    // --- STATO 4: CONNECTED ---
    case CONNECTED:
        if (checkAndSetJustEntered()) {
            log("Stato: CONNECTED (Sistema Online)");
            // Qui potresti aggiornare il Context, es: pContext->isOnline = true;
        }

        // Manteniamo viva la connessione MQTT
        client.loop();

        // Controllo errori
        if (WiFi.status() != WL_CONNECTED) {
            log("Persa connessione WiFi!");
            setState(DISCONNECTED);
        } else if (!client.connected()) {
            log("Persa connessione MQTT!");
            setState(CONNECTING_MQTT);
        }
        break;
    }
}

// --- METODI DI SUPPORTO FSM ---

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