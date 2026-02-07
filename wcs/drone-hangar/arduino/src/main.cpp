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
UserPanel* userPanel;

Scheduler sched;

void setup() {
  Serial.begin(115200);

  sched.init(BASE_PERIOD);
  Logger.log("Drone hangar");

  hw = new HWPlatform();
  hw->init();

  context = new Context();
  context->init();

  userPanel = new UserPanel(hw->getLcd());
  userPanel->init();

  Task* serialTask = new SerialMonitorTask(context);
  serialTask->init(150);

  Task* hangarTask = new HangarTask(hw,context,userPanel);
  hangarTask->init(150);

  Task* blinkingTask = new BlinkingTask(hw->getL2(),context);
  blinkingTask->init(500);

  Task* alarmTask = new AlarmTask(hw,context);
  alarmTask->init(150);

  Task* doorTask = new DoorTask(hw->getMotor(),context);
  doorTask->init(100);

  

  sched.addTask(serialTask);
  sched.addTask(hangarTask);
  sched.addTask(blinkingTask);
  sched.addTask(alarmTask);
  sched.addTask(doorTask);
}

void loop() {
  sched.schedule();
}