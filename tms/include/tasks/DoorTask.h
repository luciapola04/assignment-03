#ifndef __DOOR_TASK__
#define __DOOR_TASK__

#include "kernel/Task.h"
#include "devices/ServoMotor.h"
#include "model/Context.h"
#include <Arduino.h>

enum MotorState
{
    OPEN,
    OPENING,
    CLOSED,
    CLOSING
};

class DoorTask: public Task {

public:
  DoorTask(ServoMotor* pMotor, Context* pContext); 
  void tick();

private:  
  void setState(MotorState state);
  long elapsedTimeInState();
  void log(const String& msg);
  
  bool checkAndSetJustEntered();
  
  long stateTimestamp;
  bool justEntered;
  MotorState state;

  ServoMotor* pMotor;
  Context* pContext;

  int currentPos;
  bool toBeStopped;
};

#endif