#include "model/HWPlatform.h"
#include <Arduino.h>


void wakeUp() {}

HWPlatform::HWPlatform(){
  resetButton = new ButtonImpl(RESET_PIN);
  lcd = new  LiquidCrystal_I2C(0x27,20,4);
  L1          = new Led(L1_PIN);
  L2          = new Led(L2_PIN);
  L3          = new Led(L3_PIN);
  servoMotor  = new ServoMotorImpl(MOTOR_PIN);
  pir         = new Pir(PIR_PIN);
  sonar       = new Sonar(ECHO_PIN, TRIG_PIN, 20000); //maxTime 20000 per rilevare ogg fino a 3 metri
  tempSensor  = new TempSensorTMP36(TEMP_PIN); 
}

void HWPlatform::init(){
  pir->calibrate();
  L1->switchOn();
  L2->switchOff();
  L3->switchOff();
  servoMotor->off();
}

Button* HWPlatform::getButton(){
  return this->resetButton;
}

LiquidCrystal_I2C* HWPlatform::getLcd() {
  return this->lcd;
}

Led*  HWPlatform::getL1(){
  return this->L1;
}

Led*  HWPlatform::getL2(){
  return this->L2;
}

Led*  HWPlatform::getL3(){
  return this->L3;
}

ServoMotor* HWPlatform::getMotor(){
  return this->servoMotor;
}

Sonar* HWPlatform::getSonar(){
  return this->sonar;
}

TempSensor* HWPlatform::getTempSensor(){
  return this->tempSensor;
}

Pir* HWPlatform::getPir(){
  return this->pir;
}


void HWPlatform::test(){
}
