#ifndef __USER_PANEL__
#define __USER_PANEL__

#include "config.h"
#include "HWPlatform.h"
#include <LiquidCrystal_I2C.h>

class UserPanel {

public:
  UserPanel(LiquidCrystal_I2C* pHW);

  void init();

  void turnOffDisplay();
  void turnOnDisplay();

  void printMessage(String message);
  void printMessage(int x,int y,String message);

  void displayDroneInside();
  void displayTakeOff();
  void displayDroneOut();
  void displayLanding();
  void displayAlarm();
  
 private:
  LiquidCrystal_I2C* pLcd;
};

#endif