#ifndef __HW_PLATFORM__
#define __HW_PLATFORM__

#include "devices/ButtonImpl.h"
#include "config.h"
#include <LiquidCrystal_I2C.h>
#include "devices/Led.h"
#include "devices/Pir.h"
#include "devices/ServoMotorImpl.h"
#include "devices/Sonar.h"
#include "devices/TempSensorLM35.h"
#include "devices/TempSensorTMP36.h" 


class HWPlatform {

public:
  HWPlatform();
  void init();
  void test();

  Button* getButton();
  LiquidCrystal_I2C* getLcd();
  Led* getL1();
  Led* getL2();
  Led* getL3();
  ServoMotor* getMotor();
  Sonar* getSonar();
  TempSensor* getTempSensor();
  Pir* getPir();

private:
  Button* resetButton;
  LiquidCrystal_I2C* lcd;
  Led* L1;
  Led* L2;
  Led* L3;
  ServoMotor* servoMotor;
  Sonar* sonar;
  TempSensor* tempSensor;
  Pir* pir;
};

#endif