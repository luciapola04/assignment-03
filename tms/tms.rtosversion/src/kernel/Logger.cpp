#include "kernel/Logger.h"
#include "kernel/MsgService.h"

LoggerService Logger;

void LoggerService::log(const String& msg){
    MsgService.sendMsg("lo:"+msg);
}



