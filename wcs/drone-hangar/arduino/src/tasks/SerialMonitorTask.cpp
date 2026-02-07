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

      Logger.log("Received cmd: " + content);

      if (content == "takeoff-req"){
        pContext->setTakeOffRequest(true);
      } 
      else if (content == "landing-req"){
        pContext->setLandingRequest(true);
      }
      
      delete msg; 
    }
  }
}

void SerialMonitorTask::sendSystemState(){
  
  String droneStateStr = "UNKNOWN";
  String hangarStateStr = "NORMAL";

  if (pContext->isInAlarm()) {
    hangarStateStr = "ALARM";
  } else if (pContext->isInPreAlarm()) {
    hangarStateStr = "PRE-ALARM";
  }


  if (pContext->isInTakeOff()) {
    droneStateStr = "TAKE OFF";
  }else if(pContext->isInLanding()) {
        droneStateStr = "LANDING";
  } 
  else if (pContext->isDroneInside()) {
    droneStateStr = "REST";
  } 
  else {
    droneStateStr = "OPERATING";
  }

  String distance = "--"; 
  if(pContext->isInLanding()){
    distance = String(pContext->getDroneDistance());
  }

  String msg = "dh:st:" + droneStateStr + ":" + hangarStateStr + ":" + String(distance);

  MsgService.sendMsg(msg);
}