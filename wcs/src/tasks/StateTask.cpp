#include "tasks/StateTask.h"
#include <Arduino.h>
#include "kernel/Logger.h"
#include "config.h"

StateTask::StateTask(HWPlatform* pHW, Context* pContext, UserPanel* pUserPanel): 
    pHw(pHW), pContext(pContext), pUserPanel(pUserPanel) {
    precPressed = false;
    pContext->setManualState(LOCAL);
    setState(AUTOMATIC);
}
  
void StateTask::tick(){

    if (!pContext->isConnected() && state != UNCONNECTED) {
        setState(UNCONNECTED);
    } else if (pContext->isConnected() && state == UNCONNECTED) {
        setState(AUTOMATIC);
    }

    bool isPressed = pHw->getButton()->isPressed();
    if (isPressed && !precPressed) {
        if (state == MANUAL) {
            if (pHw->getMotor()->isOn()) {
                pHw->getMotor()->off();
            }
            setState(AUTOMATIC);
        } else {
            pHw->getMotor()->off();
            setState(MANUAL);
        }
    }
    precPressed = isPressed;

    switch (state){   

        case MANUAL: {
            if(this->checkAndSetJustEntered()) {
                Logger.log("Dentro MANUAL");
                pContext->setWCSState(MANUAL);
                lastState = MANUAL;
                pHw->getMotor()->on();
                oldValue = -1;
            }

            switch (pContext->getManualState())
            {
            case LOCAL:
                int currentValue = pHw->getPot()->getValue();

                if (abs(currentValue - oldValue) > 2) {

                    int servoAngle = map(currentValue, 1, 1020, 0, 90);

                    int perc = map(currentValue, 1, 1020, 0, 100);

                    oldValue = currentValue;

                    pHw->getMotor()->setPosition(servoAngle);

                    pContext->setValve(perc);
                }
                break;
            
            case REMOTE:

                readCommand();

                break;
            }

            break;
        }

        case AUTOMATIC: {
            if(this->checkAndSetJustEntered()) {
                Logger.log("Dentro AUTOMATIC");
                pContext->setWCSState(AUTOMATIC);
                lastState = AUTOMATIC;
                pHw->getMotor()->on();
            }

            readCommand();

            break;
        }

        case UNCONNECTED: {
            if(this->checkAndSetJustEntered()) {
                Logger.log("Dentro UNCONNECTED");
                pContext->setWCSState(UNCONNECTED);
            }

            break;
        }
    }

    updateDisplay();
}

void StateTask::readCommand() {

    int currentValue = pContext->getValvePerc();

    if (currentValue != oldValue) {

        int servoAngle = map(currentValue, 0, 100, 0, 90);

        pHw->getMotor()->setPosition(servoAngle);

        oldValue = currentValue;

    }
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