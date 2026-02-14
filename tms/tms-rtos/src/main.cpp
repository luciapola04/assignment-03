#include <Arduino.h>
#include "model/HWPlatform.h"
#include "model/Context.h"
#include "kernel/Scheduler.h"
#include "tasks/StatusTask.h"
#include "tasks/ConnectionTask.h"
#include "kernel/Logger.h"

HWPlatform* hw;
Context* context;

Scheduler sched;

void setup() {
  Serial.begin(115200);
  
  Logger.log("ESP32-TMS");

  hw = new HWPlatform();
  hw->init();

  context = new Context();
  context->init();

  Task* statusTask = new StatusTask(context,hw->getLedALARM(),hw->getLedOK(),hw->getSonar());
  statusTask->init(400);

  Task* connectionTask = new ConnectionTask(context);
  connectionTask->init(100);

  sched.startTask(statusTask, "StatusTask", 200, 10000, 1, 1);
  sched.startTask(connectionTask, "ConnTask", 100, 10000, 1, 1);
}

void loop() {

}