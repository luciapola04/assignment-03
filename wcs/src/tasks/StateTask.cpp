#include "tasks/StateTask.h"
#include <Arduino.h>
#include "kernel/Logger.h"
#include "config.h"

StateTask::StateTask(HWPlatform* pHW, Context* pContext, UserPanel* pUserPanel): 
    pHw(pHW), pContext(pContext), pUserPanel(pUserPanel) {
    precPressed = false;
    setState(UNCONNECTED);
    pHw->getMotor()->on();
}
  
void StateTask::tick(){

    WCSState globalState = pContext->getWCSState();

    if (globalState != state) {
        state = globalState;
        setState(state);  
    }

    if (!pContext->isConnected() && state != UNCONNECTED) {
        setState(UNCONNECTED);
    }

    bool isPressed = pHw->getButton()->isPressed();
    if (isPressed && !precPressed) {
        if (pContext->isConnected()) {
            if (state == MANUAL) {
                setState(AUTOMATIC);
            } else {
                setState(MANUAL);
            }
        }
    }
    precPressed = isPressed;

    switch (state){   

        case MANUAL: {
            if(this->checkAndSetJustEntered()) {
                Logger.log("Dentro MANUAL");
                oldValue = pHw->getPot()->getValue();
                pContext->setManualState(LOCAL);
            }

            int currentValue = pHw->getPot()->getValue();
            if (abs(currentValue - oldValue) > 15) {
                if (pContext->getManualState() == REMOTE) {
                    Logger.log("Override Manuale rilevato: Passaggio a LOCAL");
                    pContext->setManualState(LOCAL);
                }
                oldValue = currentValue;
            }

            switch (pContext->getManualState())
            {
                case LOCAL: {

                    int servoAngle = map(currentValue, 1, 1020, 0, 90);
                    int perc = map(currentValue, 1, 1020, 0, 100);

                    oldValue = currentValue;

                    pHw->getMotor()->setPosition(servoAngle);
                    pContext->setValve(perc);

                    break;
                } 
                
                case REMOTE: {

                    setMotorAnglePerc();

                    break;

                }
            }

            break;
        }

        case AUTOMATIC: {
            if(this->checkAndSetJustEntered()) {
                Logger.log("Dentro AUTOMATIC");
            }

            setMotorAnglePerc();

            break;
        }

        case UNCONNECTED: {
            if(this->checkAndSetJustEntered()) {
                Logger.log("Dentro UNCONNECTED");
                pContext->setValve(100);
            }

            if (pContext->isConnected())
            {
                setState(AUTOMATIC);
                pContext->setValve(0);
            }

            break;
        }

    }

    updateDisplay();

    
}

void StateTask::setMotorAnglePerc() {

    int currentValue = pContext->getValvePerc();
    int servoAngle = map(currentValue, 0, 100, 0, 90);
    pHw->getMotor()->setPosition(servoAngle);
    

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
    pContext->setWCSState(s);
    state = s;
    stateTimestamp = millis();
    justEntered = true;
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