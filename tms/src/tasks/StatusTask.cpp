#include "tasks/StatusTask.h"
#include <Arduino.h>
#include "config.h"

StatusTask::StatusTask(Context* pContext): 
    pContext(pContext){
    setState(ALARM);
}
  
void StatusTask::tick(){
    switch (state){   
        case OK: {
            if (this->checkAndSetJustEntered()){
                
            }

            break;
        }
        case ALARM: {
            if (this->checkAndSetJustEntered()){
            }
            

            break;
        }
    }
}


void StatusTask::setState(StatusState s){
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long StatusTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

bool StatusTask::checkAndSetJustEntered(){
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}
