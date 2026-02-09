#include <Arduino.h>
#include "model/HWPlatform.h"
#include "model/Context.h"
#include "tasks/StateTask.h"
#include "tasks/SerialMonitorTask.h"
#include "kernel/Scheduler.h"
#include "kernel/Logger.h"

#define BASE_PERIOD 50

HWPlatform* hw;
Context* context;
UserPanel* userPanel;

Scheduler sched;

void setup() {
  Serial.begin(115200);

  sched.init(BASE_PERIOD);
  Logger.log("Water Channel System");

  hw = new HWPlatform();
  hw->init();

  context = new Context();
  context->init();

  userPanel = new UserPanel(hw->getLcd());
  userPanel->init();

  Task* serialTask = new SerialMonitorTask(context);
  serialTask->init(150);

  Task* stateTask = new StateTask(hw, context, userPanel);
  stateTask->init(150);

  sched.addTask(stateTask);
  sched.addTask(serialTask);
}

void loop() {
  sched.schedule();
}