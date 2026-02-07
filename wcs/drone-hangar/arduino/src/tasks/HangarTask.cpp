#include "tasks/HangarTask.h"
#include <Arduino.h>
#include "kernel/Logger.h"
#include "config.h"

HangarTask::HangarTask(HWPlatform* pHW, Context* pContext, UserPanel* pUserPanel): 
    pHw(pHW), pContext(pContext), pUserPanel(pUserPanel) {
    setState(IDLE);
}
  
void HangarTask::tick(){

    if (pContext->isInAlarm() && state != ALARM_STATE) {
        setState(ALARM_STATE);
    }

    switch (state){   

        case IDLE: {
            if(this->checkAndSetJustEntered()) {
                Logger.log("Dentro IDLE");

                if(pContext->isDroneInside()) {
                    pUserPanel->printMessage("DRONE INSIDE");
                } else {
                    pUserPanel->printMessage("DRONE OUTSIDE");
                }
            }

            if(!pContext->isInPreAlarm()){

                if (pContext->isTakeOffRequest()) {
                    Logger.log("Richiesta TAKE OFF");
                    if (pContext->isDroneInside()) {
                        setState(TAKE_OFF);
                    } else {
                        Logger.log("ERRORE: Drone gia' fuori!");
                    }
                    pContext->setTakeOffRequest(false);

                } else if (pContext->isLandingRequest()){

                    Logger.log("Richiesta LANDING");
                    pHw->getPir()->sync();
                    if (!pContext->isDroneInside() && pHw->getPir()->isDetected()) {
                        setState(LANDING);
                    } else {
                        Logger.log("ERRORE: Drone dentro o non rilevato dal PIR!");
                    }
                    pContext->setLandingRequest(false);
                }
            }

            break;
        }

        case TAKE_OFF: {

            if (this->checkAndSetJustEntered()) {
                Logger.log("Dentro TAKEOFF");
                pUserPanel->printMessage("TAKE OFF");
                pContext->setTakeOff(true);
            }

            if (pContext->isDoorOpen()) {

                float currentDist = pHw->getSonar()->getDistance();
                Logger.log("[WC] Dist: " + String(currentDist).substring(0,5));

                if (currentDist > D1) {
                    if (!sensorConditionMet) {
                        sensorConditionStartTime = millis();
                        sensorConditionMet = true;
                    }else{

                        if ((millis() - sensorConditionStartTime) > T1) {
                        Logger.log("Drone Uscito. Chiudo...");
                        pContext->setDroneInside(false);
                        pContext->setTakeOff(false);
                        setState(IDLE);
                        }
                    }
                    
                } else {
                    sensorConditionMet = false;
                }
            }
            break;
        }

        case LANDING: {
            if (this->checkAndSetJustEntered()) {
                Logger.log("Dentro LANDING");
                pUserPanel->printMessage("LANDING");
                pContext->setLanding(true);
            }

            if (pContext->isDoorOpen()) {

                float currentDist = pHw->getSonar()->getDistance();
                pContext->setDroneDistance(currentDist);
                Logger.log("[WC] Dist: " + String(currentDist).substring(0,5));

                if (currentDist < D2 && currentDist >=0) {
                    if (!sensorConditionMet) {
                        sensorConditionStartTime = millis();
                        sensorConditionMet = true;
                    }else{

                        if ((millis() - sensorConditionStartTime) > T2) {
                            Logger.log("Drone Entrato. Chiudo...");
                            pContext->setLanding(false);
                            pContext->setDroneInside(true);
                            setState(IDLE);
                        }

                    }
                } else {
                    sensorConditionMet=false;
                }
            }
            break;
        }

        case ALARM_STATE: {
            if (this->checkAndSetJustEntered()) {
                Logger.log("ALLARME!!!");
                pUserPanel->printMessage("ALLARM");
                pContext->setLanding(false);
                pContext->setTakeOff(false);
            }

            if (!pContext->isInAlarm()) {
                setState(IDLE);
            }
            break;

        }
    }
}


void HangarTask::setState(HangarState s){
    state = s;
    stateTimestamp = millis();
    justEntered = true;
    sensorConditionMet = false;
}

long HangarTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

bool HangarTask::checkAndSetJustEntered(){
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}