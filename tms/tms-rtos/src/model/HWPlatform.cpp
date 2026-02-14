#include "model/HWPlatform.h"
#include <Arduino.h>


void wakeUp() {}

HWPlatform::HWPlatform(){
  LOK          = new Led(LEDOK_PIN);
  LALARM          = new Led(LEDALARM_PIN);
  sonar       = new Sonar(ECHO_PIN, TRIG_PIN, 20000); //maxTime 20000 per rilevare ogg fino a 3 metri
}

void HWPlatform::init(){
  LALARM->switchOn();
  LOK->switchOff();
}

Led*  HWPlatform::getLedALARM(){
  return this->LALARM;
}

Led*  HWPlatform::getLedOK(){
  return this->LOK;
}


Sonar* HWPlatform::getSonar(){
  return this->sonar;
}

void HWPlatform::test(){
}
