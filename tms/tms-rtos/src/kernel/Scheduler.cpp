#include "kernel/Scheduler.h"

Scheduler::Scheduler() {
    this->nTasks = 0;
    for(int i = 0; i < MAX_TASKS; i++) {
        this->runners[i] = NULL;
    }
}

void Scheduler::taskEntry(void* pvParameters) {
    TaskRunner* runner = (TaskRunner*)pvParameters;
    Task* task = runner->task;
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency =runner->period;

    for ( ; ; ) {
        task->tick();
        xLastWakeTime = xTaskGetTickCount();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

bool Scheduler::startTask(Task* task, const char* name, int periodMs, uint32_t stackSize, int priority, int coreId) {
    
    if (this->nTasks >= MAX_TASKS) {
        Serial.println("ERRORE SCHEDULER: Numero massimo di task raggiunto!");
        return false;
    }

    this->runners[this->nTasks] = new TaskRunner{task, periodMs};

    BaseType_t result = xTaskCreatePinnedToCore(
        Scheduler::taskEntry,        
        name,                        
        stackSize,                   
        this->runners[this->nTasks], 
        priority,                    
        NULL,                       
        coreId                       
    );

    if (result == pdPASS) {
        this->nTasks++;
        return true;
    } else {
        Serial.println("ERRORE FREERTOS: Impossibile creare il task (Memoria insufficiente?)");
        delete this->runners[this->nTasks];
        this->runners[this->nTasks] = NULL;
        return false;
    }
}