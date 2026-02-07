#ifndef __STATUS_TASK__
#define __STATUS_TASK__

#include "kernel/Task.h"
#include "model/Context.h"
#include "devices/Led.h"
#include <Arduino.h>

class StatusTask: public Task {

public:
  StatusTask(Context* pContext); 
  void tick();

private:
  enum StatusState { OK, ALARM};
  void setState(StatusState state);
  long elapsedTimeInState();
  void log(const String& msg);
  
  bool checkAndSetJustEntered();
  
  long stateTimestamp;
  bool justEntered;

  Context* pContext;
  StatusState state;
};

#endif