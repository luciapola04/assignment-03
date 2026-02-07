#include "HWPlatform.h"
#include "UserPanel.h"
#ifndef __CONTEXT__
#define __CONTEXT__

enum WCSState {MANUAL, AUTOMATIC, UNCONNECTED};

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

    private:
        WCSState wcsState;

        int valvePerc;
        bool connected;
        
    };
    
#endif