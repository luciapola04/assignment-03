#include "kernel/Logger.h"
#include "kernel/MsgService.h"

void LoggerService::log(const String& msg){
    MsgService.sendMsg("lo:"+msg);
}



