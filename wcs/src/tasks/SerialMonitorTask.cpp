#include "tasks/SerialMonitorTask.h"
#include "kernel/MsgService.h"
#include "kernel/Logger.h"
#include <Arduino.h>

SerialMonitorTask::SerialMonitorTask(Context* pContext): pContext(pContext){
}

void SerialMonitorTask::init(int period){
  Task::init(period);
  MsgService.init();
}

void SerialMonitorTask::tick(){
  
  
  checkSerialMonitor();

  if (millis() - this->lastTimeUpdate > 300) {
        sendSystemState();
        this->lastTimeUpdate = millis();
  }
  
}

void SerialMonitorTask::checkSerialMonitor(){

  if (MsgService.isMsgAvailable()) {
    Msg* msg = MsgService.receiveMsg();    
    if (msg != NULL){
      String content = msg->getContent();
      content.trim();

      if (content.length() == 0) {
        delete msg;
        return; 
      }

      // v: ->>> percentuale apertuta, m: modalità
      String cleanContent = content;

      if (content.startsWith("m:")) {
        cleanContent.replace("m:", "");
        cleanContent.trim();
        int val = cleanContent.toInt();
        if (val == UNCONNECTED_VAL) {
          pContext->setConnession(false);
        } else if(val == AUTOMATIC_VAL){
          pContext->setWCSState(AUTOMATIC);
          pContext->setConnession(true);
        } else if(val == MANUAL_VAL){
          pContext->setWCSState(MANUAL);
          pContext->setConnession(true);
        }
      } else if (content.startsWith("v:")) {
        cleanContent.replace("v:", "");
        cleanContent.trim();
        int val = cleanContent.toInt();
        if (val >= MIN_PERC && val <= MAX_PERC){
          Logger.log("Received valve opening: " + String(val )+ "%");
          pContext->setValve(val);
        } else {
          Logger.log("Valore apertura valvola invalido o fuori range!");
        }
      }

      delete msg; 
    }
  }
}

void SerialMonitorTask::sendSystemState(){
  
  String WCSStateStr = "UNKNOWN";

  if (!pContext->isConnected()) {
    WCSStateStr = "UNCONNECTED";
  } else if(pContext->getWCSState() == MANUAL) {
    WCSStateStr = "MANUAL";
  } else if(pContext->getWCSState() == AUTOMATIC){
    WCSStateStr = "AUTOMATIC";
  }

  String valveOpening = ""; 
  valveOpening = String(pContext->getValvePerc());

  String msg = "st:vo:" + WCSStateStr + ":" + valveOpening;
  
  MsgService.sendMsg(msg);
}

