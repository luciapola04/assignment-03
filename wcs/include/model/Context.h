#include "HWPlatform.h"
#include "UserPanel.h"
#ifndef __CONTEXT__
#define __CONTEXT__

enum WCSState {MANUAL, AUTOMATIC, UNCONNECTED, LOCAL, REMOTE};

class Context {
    public:
        Context();
        void init();

        void setWCSState(WCSState state);
        WCSState getWCSState();

        void setValve(int perc); //in automatica lo setta il CUS
        int getValvePerc();

        void setConnession(bool connession); //lo setta il CUS
        bool isConnected();

        void setManualState(WCSState manualState);
        WCSState getManualState();

    private:
        WCSState wcsState;
        WCSState manualState;

        int valvePerc;
        bool connected;
        
    };
    
#endif