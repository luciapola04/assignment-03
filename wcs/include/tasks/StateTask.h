#ifndef __STATE_TASK__
#define __STATE_TASK__

#include "kernel/Task.h"
#include "model/HWPlatform.h"
#include "model/Context.h"


class StateTask: public Task {
public:
StateTask(HWPlatform* pHw,Context* pContext,UserPanel* pUserPanel);
  virtual void tick();

private:
  void setState(WCSState s);
  long elapsedTimeInState();
  bool checkAndSetJustEntered();
  void updateDisplay();

  HWPlatform* pHw;
  Context* pContext;
  UserPanel* pUserPanel;

  int oldPotValue;
  bool isClosing;
  long doorTimer;

  WCSState state;
  long stateTimestamp;
  long sensorConditionStartTime;
  bool justEntered;  
  bool sensorConditionMet;
};

#endif