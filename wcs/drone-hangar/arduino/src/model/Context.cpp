#include "model/Context.h"

Context::Context(){
    this->init();
}

void Context::init(){
    this->alarmState = AT_NORMAL;
    this->currentTemp = 0.0;
    this->currentDistance = 0.0;
    this->dronePresent = false;
    this->droneInside = true;
    this->inPreAlarm = false;
    this->doorOpen = false;
    this->landing=false;
    this->takeOff=false;
    this->reqTakeOff = false;
    this->reqLanding = false;

    this -> currentDistance = 0;
    this -> currentTemp = 15;
}


void Context::setAlarmState(AlarmState s) { this->alarmState = s; }
AlarmState Context::getAlarmState() { return this->alarmState; }

float Context::getCurrentTemp() { return this->currentTemp; }
float Context::getDroneDistance() { return this->currentDistance; }
void Context::setDroneDistance(float d) { this->currentDistance = d; }
void Context::setDronePresent(bool present) { this->dronePresent = present; }
bool Context::isDronePresent() { return this->dronePresent; }
bool Context::isDroneInside() { return this->droneInside; }
void Context::setDroneInside(bool inside) { this->droneInside = inside; }

bool Context::isInPreAlarm() { return this->inPreAlarm; }
void Context::setPreAlarm(bool preAlarm){ this->inPreAlarm = preAlarm;}
bool Context::isInAlarm() {return this->inAlarm;}
void Context::setAlarm(bool alarm){ this->inAlarm = alarm;}

bool Context::isTakeOffRequest() { return this->reqTakeOff; }
void Context::setTakeOffRequest(bool req) { this->reqTakeOff = req; }
bool Context::isLandingRequest() { return this->reqLanding; }
void Context::setLandingRequest(bool req) { this->reqLanding = req; }
float Context::getTemperature() { return this->currentTemp;}

bool Context::isDoorOpen() {return this->doorOpen;}
void Context::setDoorOpen(bool door) { this->doorOpen = door; }

bool Context::isInLanding() {return this->landing;}
bool Context::isInTakeOff() {return this->takeOff;}
void Context::setLanding(bool landing) {this->landing=landing;}
void Context::setTakeOff(bool takeOff) {this->takeOff=takeOff;}