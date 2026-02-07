#include "tasks/DoorTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"

DoorTask::DoorTask(ServoMotor* pMotor, Context* pContext): 
    pMotor(pMotor), pContext(pContext){
    setState(CLOSED);
}

void DoorTask::tick(){

    bool doorAction = (pContext->isInLanding() || pContext->isInTakeOff());

    switch (state){
        case OPEN: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("DOOR OPEN"));
                pContext->setDoorOpen(true);
            }

            if (!doorAction || pContext->isInAlarm()) {
                setState(CLOSING);
            }

        break;
    }
    
        case OPENING: {        
            if (this->checkAndSetJustEntered()){
                Logger.log(F("DOOR IS OPENING..."));
                pMotor->on();
            }
            
            long elapsed = this->elapsedTimeInState();
            if (elapsed < T_DOOR_MOVE) {
                int pos = map(elapsed, 0, T_DOOR_MOVE, MOTOR_CLOSE_POS, MOTOR_OPEN_POS);
                pMotor->setPosition(pos);
            } else {
                pMotor->setPosition(MOTOR_OPEN_POS);
                pMotor->off();
                setState(OPEN);
            }
            break;
        }

        case CLOSED: {   
            if (this->checkAndSetJustEntered()){
                Logger.log(F("DOOR CLOSED"));
                pContext->setDoorOpen(false);
            }

            if (doorAction) {
                setState(OPENING);
            }
            break;
        }

        case CLOSING: {        
            if (this->checkAndSetJustEntered()){
                Logger.log(F("DOOR IS CLOSING..."));
                pMotor->on();
            }
            
            long elapsed = this->elapsedTimeInState();
            if (elapsed < T_DOOR_MOVE) {
                int pos = map(elapsed, 0, T_DOOR_MOVE, MOTOR_OPEN_POS, MOTOR_CLOSE_POS);
                pMotor->setPosition(pos);
            } else {
                pMotor->setPosition(MOTOR_CLOSE_POS);
                pMotor->off();
                setState(CLOSED);
            }
            break;
        }
    }
}

void DoorTask::setState(MotorState s){
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long DoorTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

bool DoorTask::checkAndSetJustEntered(){
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}
