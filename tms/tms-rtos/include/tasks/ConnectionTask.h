#ifndef __CONNECTION_TASK__
#define __CONNECTION_TASK__

#include "kernel/Task.h"
#include "model/Context.h"
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

class ConnectionTask: public Task {

public:

  ConnectionTask(Context* pContext); 
  void tick();


private:  

  
  const char* ssid = "iPhone di Leonardo";
  const char* password = "00000000";
  const char* mqtt_server = "broker.mqtt-dashboard.com";

  enum ConnectionState {DISCONNECTED,CONNECTING_MQTT,CONNECTED};
  void setState(ConnectionState state);
  long elapsedTimeInState();
  void log(const String& msg);
  bool checkAndSetJustEntered();
  
  ConnectionState state;
  long stateTimestamp;
  bool justEntered;
  Context* pContext;


};

#endif