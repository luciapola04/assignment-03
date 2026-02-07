#include <Arduino.h>
#include "model/HWPlatform.h"
#include "model/Context.h"
#include "tasks/HangarTask.h"
#include "tasks/SerialMonitorTask.h"
#include "tasks/AlarmTask.h"
#include "tasks/BlinkingTask.h"
#include "tasks/DoorTask.h"
#include "kernel/Scheduler.h"
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

  Task* doorTask = new AlarmTask(hw->getMotor(),context);
  doorTask->init(100);

  

  sched.addTask(serialTask);
}

void loop() {
  sched.schedule();
}