#ifndef __HANGAR_TASK__
#define __HANGAR_TASK__

#include "kernel/Task.h"
#include "model/HWPlatform.h"
#include "model/Context.h"


class HangarTask: public Task {
public:
  HangarTask(HWPlatform* pHw,Context* pContext,UserPanel* pUserPanel);
  virtual void tick();

private:
  void setState(HangarState s);
  long elapsedTimeInState();
  bool checkAndSetJustEntered();

  HWPlatform* pHw;
  Context* pContext;
  UserPanel* pUserPanel;

  bool isClosing;
  long doorTimer;

  HangarState state;
  long stateTimestamp;
  long sensorConditionStartTime;
  bool justEntered;  
  bool sensorConditionMet;
};

#endif