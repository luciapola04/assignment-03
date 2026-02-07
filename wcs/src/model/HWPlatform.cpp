#include "model/HWPlatform.h"
#include <Arduino.h>


void wakeUp() {}

HWPlatform::HWPlatform(){
  button = new ButtonImpl(BUTTON_PIN);
  lcd = new  LiquidCrystal_I2C(0x27,20,4);
  servoMotor  = new ServoMotorImpl(MOTOR_PIN);
  pot = new Potentiometer(POT_PIN);
}

void HWPlatform::init(){
  this->servoMotor->off();
}

Button* HWPlatform::getButton(){
  return this->button;
}

LiquidCrystal_I2C* HWPlatform::getLcd() {
  return this->lcd;
}

ServoMotor* HWPlatform::getMotor(){
  return this->servoMotor;
}

Potentiometer* HWPlatform::getPot() {
  return this->pot;
}

void HWPlatform::test(){
}
