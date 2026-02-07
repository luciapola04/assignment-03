#include "model/Context.h"

Context::Context(){
    this->init();
}

void Context::init(){
    this->wcsState = MANUAL;
    this->valvePerc = 0;
    this->connected = true;
}

void Context::setWCSState(WCSState state) { this->wcsState = state; }
WCSState Context::getWCSState() { return this->wcsState; }

void Context::setValve(int perc) { this->valvePerc = perc; }
int Context::getValvePerc() { return this->valvePerc; }

void Context::setConnession(bool connession) { this->connected = connession; }
bool Context::isConnected() { return this->connected; }