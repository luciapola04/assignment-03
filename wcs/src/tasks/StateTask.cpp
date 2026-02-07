#include "tasks/StateTask.h"
#include <Arduino.h>
#include "kernel/Logger.h"
#include "config.h"

StateTask::StateTask(HWPlatform* pHW, Context* pContext, UserPanel* pUserPanel): 
    pHw(pHW), pContext(pContext), pUserPanel(pUserPanel) {
    setState(MANUAL);
}
  
void StateTask::tick(){

    if (!pContext->isConnected()) {
        setState(UNCONNECTED);
    }

    if (pHw->getButton()->isPressed()) {
        if (state == MANUAL) {
            if (pHw->getMotor()->isOn()) {
                pHw->getMotor()->off();
            }
            setState(AUTOMATIC);
        } else {
            setState(MANUAL);
        }
    }

    switch (state){   

        case MANUAL: {
            if(this->checkAndSetJustEntered()) {
                Logger.log("Dentro MANUAL");
                pHw->getMotor()->on();
                oldPotValue = -1;
            }

            int currentValue = pHw->getPot()->getValue();

            Logger.log((String)pHw->getPot()->getValue());

            if (abs(currentValue - oldPotValue) > 2) {

                int servoAngle = map(currentValue, 1, 1020, 0, 90);

                int perc = map(currentValue, 1, 1020, 0, 100);

                oldPotValue = currentValue;

                pHw->getMotor()->setPosition(servoAngle);

                pContext->setValve(perc);
            }

            break;
        }

        case AUTOMATIC: {
            if(this->checkAndSetJustEntered()) {
                Logger.log("Dentro AUTOMATIC");
            }

            break;
        }

        case UNCONNECTED: {
            if(this->checkAndSetJustEntered()) {
                Logger.log("Dentro UNCONNECTED");
            }

            break;
        }
    }

    updateDisplay();
}

void StateTask::updateDisplay() {
    
    pUserPanel->clearDisplay();

    String modeText = "";
    if (state == MANUAL) {
        modeText = "MANUAL";
    } else if (state == AUTOMATIC) {
        modeText = "AUTOMATIC";
    } else {
        modeText = "UNCONNECTED";
    }

    String stateStr = "MODE: " + modeText;

    pUserPanel->printMessage(0, 0, stateStr);

    String valvePerc = String(pContext->getValvePerc()) + "%";

    pUserPanel->printMessage(1, 1, valvePerc);
}

void StateTask::setState(WCSState s){
    state = s;
    stateTimestamp = millis();
    justEntered = true;
    sensorConditionMet = false;
}

long StateTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

bool StateTask::checkAndSetJustEntered(){
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}