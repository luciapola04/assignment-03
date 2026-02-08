#include "tasks/StatusTask.h"
#include <Arduino.h>
#include "config.h"
#include <kernel/Logger.h>

StatusTask::StatusTask(Context* pContext,Led* ledKO, Led* ledOK,Sonar* sonar): 
    pContext(pContext),ledKO(ledKO),ledOK(ledOK),sonar(sonar){
    setState(ALARM);
}
  
void StatusTask::tick(){

    switch (state){   
        case OK: {
            if (this->checkAndSetJustEntered()){
                this->ledOK->switchOn();
                this->ledKO->switchOff();
            }

            float levelWater = this->sonar->getDistance();
            snprintf (msg, MSG_BUFFER_SIZE, "Distance:%.2f", levelWater);
            Serial.println(String("Publishing message: ") + msg);
            bool mqttSending = this->pContext->getMQTTClient()->publish(mqtt_topic, msg);  

            if(!mqttSending){
                this->pContext->setMqttError(true);
            }

            if(!pContext->isSystemOnline()){
                setState(ALARM);
            }

            break;
        }
        case ALARM: {
            if (this->checkAndSetJustEntered()){
                log("Allarme");
                this->ledKO->switchOn();
                this->ledOK->switchOff();
            }
            
            if(pContext->isSystemOnline()){
                setState(OK);
            }
            break;
        }
    }
}


void StatusTask::setState(StatusState s){
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long StatusTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

bool StatusTask::checkAndSetJustEntered(){
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}

void StatusTask::log(const String& msg) {
    Logger.log("[StatusTask] " + msg);
}
