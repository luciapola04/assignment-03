#include "devices/ServoMotorImpl.h"
#include "Arduino.h"
#include "config.h"

ServoMotorImpl::ServoMotorImpl(int pin){
  this->pin = pin;  
  _on = false;
} 

void ServoMotorImpl::on(){
  // updated values: min is 544, max 2400 (see ServoTimer2 doc)
  motor.attach(pin); //, 544, 2400); 
  this->setPosition(MOTOR_CLOSE_POS);   
  _on = true;
  delay(50); //per permettere al servo di stabilizzarsi
}

bool ServoMotorImpl::isOn(){
  return _on;
}

void ServoMotorImpl::setPosition(int angle){
	if (angle > 180){
		angle = 180;
	} else if (angle < 0){
		angle = 0;
	}
  // 750 -> 0, 2250 -> 180 
  // 750 + angle*(2250-750)/180
  // updated values: min is 544, max 2400 (see ServoTimer2 doc)
  float coeff = (2250.0-750.0)/180;
  motor.write(750 + angle*coeff);              
}

void ServoMotorImpl::off(){
  _on = false;
  motor.detach();    
}
