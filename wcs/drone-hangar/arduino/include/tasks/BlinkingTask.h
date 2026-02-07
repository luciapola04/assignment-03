#ifndef __BLINKING_TASK__
#define __BLINKING_TASK__

#include "kernel/Task.h"
#include "model/Context.h"
#include "devices/Led.h"
#include <Arduino.h>

class BlinkingTask: public Task {

public:
  BlinkingTask(Led* pLed, Context* pContext); 
  void tick();

private:
  enum State { IDLE, OFF, ON };
  void setState(State state);
  long elapsedTimeInState();
  void log(const String& msg);
  
  bool checkAndSetJustEntered();
  
  long stateTimestamp;
  bool justEntered;

  Led* pLed;
  Context* pContext;
  State state;
};

#endif