#include "model/Context.h"

Context::Context(){
    this->init();
}

void Context::init(){
   this->online = false;
   this->mqttError = false;
   this->mutex = xSemaphoreCreateMutex();
}

bool Context::isSystemOnline(){
    return safeGet(this->online);
}

void Context::setSystemOnline(bool status){
    safeSet(this->online,status);
}

void Context::setMQTTClient(PubSubClient* client){
    safeSet(this->mqttClient,client);
}

PubSubClient* Context::getMQTTClient(){
    return safeGet(this->mqttClient);
}

bool Context::isMqttError(){return safeGet(this->mqttError);}
void Context::setMqttError(bool status){safeSet(this->mqttError,status);}


