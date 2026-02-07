#ifndef __ALARM_TASK__
#define __ALARM_TASK__

#include "kernel/Task.h"
#include "model/Context.h"

class AlarmTask: public Task {

public:
  AlarmTask(HWPlatform* pHw,Context* pContext); 
  void tick();

private:  
  void setState(AlarmState state);
  long elapsedTimeInState();
  void log(const String& msg);
  
  bool checkAndSetJustEntered();
  
  AlarmState state;
  long stateTimestamp;
  bool justEntered;

  Context* pContext;
  HWPlatform* pHw;

  float currentTemp;
  bool buttonReset;
};

#endif