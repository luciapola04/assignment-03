#include "tasks/AlarmTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"
#include "model/Context.h"



AlarmTask::AlarmTask(HWPlatform* pHw, Context* pContext): 
    pContext(pContext), pHw(pHw) {
    setState(AT_NORMAL);
}
  
void AlarmTask::tick(){
    
    this->currentTemp = pHw->getTempSensor()->getTemperature();
    static int localCounter = 0;    
    localCounter++;
    if (localCounter % 50 == 0){
        Logger.log("[WC] Temp: " + String(currentTemp).substring(0,5));
    }

    switch (state){    
    case AT_NORMAL: {
        if (checkAndSetJustEntered()){
            Logger.log(F("[ALARM] System Normal"));
            pContext->setPreAlarm(false);
            pContext->setAlarm(false);
        }
        
        if (this->currentTemp >= Temp2){
            setState(CHECKING_ALARM);
        }
        if (this->currentTemp >= Temp1){
            setState(CHECKING_PRE_ALARM);
        }
        break;
    }

    case CHECKING_PRE_ALARM: {
        
        if (this->currentTemp >= Temp2){
            setState(CHECKING_ALARM);
        }
        if (this->currentTemp < Temp1){
            setState(AT_NORMAL);
        } 
        else if (elapsedTimeInState() > T3){
            setState(PRE_ALARM);
        }
        break;       
    }

    case PRE_ALARM: {
        if (checkAndSetJustEntered()){
            Logger.log(F("[ALARM] Pre-Alarm State"));
            pContext->setPreAlarm(true);
        }
        if (this->currentTemp < Temp1){
            setState(AT_NORMAL);
        }
        else if (this->currentTemp >= Temp2){
            setState(CHECKING_ALARM);
        }
        break;
    }

    case CHECKING_ALARM: {
        if (this->currentTemp < Temp2){
            setState(PRE_ALARM);
        }
        else if (elapsedTimeInState() > T4){
            setState(ALARM);
        }
        break;
    }

    case ALARM: {
        if (checkAndSetJustEntered()){
            Logger.log(F("[ALARM] CRITICAL ALARM!"));
            pContext->setPreAlarm(false);
            pContext->setAlarm(true);
            pHw->getL3()->switchOn();
            pHw->getL1()->switchOff();
        }

        if (pHw->getButton()->isPressed()){
            Logger.log(F("[ALARM] Reset pressed."));
            pContext->setAlarm(false);
            pHw->getL3()->switchOff();
            pHw->getL1()->switchOn();
            setState(AT_NORMAL);
        }
        break;
    }
    
    }
}

void AlarmTask::setState(AlarmState s){
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long AlarmTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

bool AlarmTask::checkAndSetJustEntered(){
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}
