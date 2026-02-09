#ifndef __SERIALTASK__
#define __SERIALTASK__

#include "kernel/Task.h"
#include "model/Context.h"

#define UNCONNECTED_VAL 1
#define AUTOMATIC_VAL 2
#define MANUAL_VAL 3
#define MIN_PERC 0
#define MAX_PERC 100

class SerialMonitorTask : public Task
{

public:
    SerialMonitorTask(Context* pContext);
    void init(int period);
    void tick();

private:
    Context* pContext;
    void sendSystemState();
    void checkSerialMonitor();
    long lastTimeUpdate=0;
};

#endif
