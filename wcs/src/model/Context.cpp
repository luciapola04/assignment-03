#include "model/Context.h"

Context::Context(){
    this->init();
}

void Context::init(){
    this->wcsState = UNCONNECTED;
    this->manualState = LOCAL;
    this->valvePerc = 0;
    this->connected = false;
}

void Context::setWCSState(WCSState state) { this->wcsState = state; }
WCSState Context::getWCSState() { return this->wcsState; }

void Context::setValve(int perc) { this->valvePerc = perc; }
int Context::getValvePerc() { return this->valvePerc; }

void Context::setConnession(bool connession) { this->connected = connession; }
bool Context::isConnected() { return this->connected; }

void Context::setManualState(ManualState manualState) { this->manualState = manualState; }
ManualState Context::getManualState() { return this->manualState; }