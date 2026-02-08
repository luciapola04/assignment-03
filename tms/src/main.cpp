#include <Arduino.h>
#include "model/HWPlatform.h"
#include "model/Context.h"
#include "kernel/Scheduler.h"
#include "tasks/StatusTask.h"
#include "tasks/ConnectionTask.h"
#include "kernel/Logger.h"

#define BASE_PERIOD 50

HWPlatform* hw;
Context* context;

Scheduler sched;

void setup() {
  Serial.begin(115200);

  sched.init(BASE_PERIOD);
  Logger.log("ESP32-TMS");

  hw = new HWPlatform();
  hw->init();

  context = new Context();
  context->init();

  Task* statusTask = new StatusTask(context,hw->getLedALARM(),hw->getLedOK(),hw->getSonar());
  statusTask->init(150);

  Task* connectionTask = new ConnectionTask(context);
  connectionTask->init(100);

  sched.addTask(statusTask);
  sched.addTask(connectionTask);
}

void loop() {
  sched.schedule();
}