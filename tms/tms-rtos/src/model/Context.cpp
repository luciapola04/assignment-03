#include "model/Context.h"

Context::Context(){
    this->init();
}

void Context::init(){
   this->online = false;
   this->mqttError = false;
}

bool Context::isSystemOnline(){
    return this->online;
}

void Context::setSystemOnline(bool status){
    this->online=status;
}

void Context::setMQTTClient(PubSubClient* client){
    this->mqttClient=client;
}

PubSubClient* Context::getMQTTClient(){
    return this->mqttClient;
}

bool Context::isMqttError(){return this->mqttError;}
void Context::setMqttError(bool status){this->mqttError = status;}


