#include "model/UserPanel.h"
#include <Arduino.h>
#include "model/HWPlatform.h"

UserPanel::UserPanel(LiquidCrystal_I2C* lcd) {
    pLcd = lcd;
}

void UserPanel::init(){
  pLcd->init();
  pLcd->backlight();
  pLcd->noDisplay();
  turnOnDisplay();
}

void UserPanel::turnOnDisplay(){
  pLcd->display();
  pLcd->clear();
}

void UserPanel::turnOffDisplay(){
  pLcd->noDisplay();
}

void UserPanel::clearDisplay(){
  pLcd->clear();
}

void UserPanel::printMessage(String message){
  pLcd->setCursor(0, 0); 
  pLcd->print(message);

}

void UserPanel::printMessage(int x,int y,String message){
  pLcd->setCursor(x, y); 
  pLcd->print(message);

}
