#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>
#include "Task.h"

#define MAX_TASKS 10
struct TaskRunner {
    Task* task;
    int period;
};

class Scheduler {
private:
    TaskRunner* runners[MAX_TASKS]; 
    int nTasks;

    static void taskEntry(void* pvParameters);

public:
    Scheduler();

    bool startTask(Task* task, const char* name, int periodMs, uint32_t stackSize, int priority, int coreId);
};

#endif