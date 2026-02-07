#ifndef __HW_PLATFORM__
#define __HW_PLATFORM__

#include "devices/ButtonImpl.h"
#include "config.h"
#include <LiquidCrystal_I2C.h>
#include "devices/ServoMotorImpl.h"
#include "devices/Pot.h"


class HWPlatform {

public:
  HWPlatform();
  void init();
  void test();

  Button* getButton();
  LiquidCrystal_I2C* getLcd();
  ServoMotor* getMotor();
  Potentiometer *getPot();

private:
  Button* button;
  LiquidCrystal_I2C* lcd;
  ServoMotor* servoMotor;
  Potentiometer *pot;
};

#endif