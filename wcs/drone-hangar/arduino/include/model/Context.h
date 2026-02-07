#include "HWPlatform.h"
#include "UserPanel.h"
#ifndef __CONTEXT__
#define __CONTEXT__

enum HangarState {IDLE, TAKE_OFF, LANDING, ALARM_STATE};
enum AlarmState { AT_NORMAL, CHECKING_PRE_ALARM, PRE_ALARM, CHECKING_ALARM, ALARM };

class Context {
    public:
        Context();
        void init();

        void setAlarmState(AlarmState s);
        AlarmState getAlarmState();

        float getCurrentTemp();

        float getDroneDistance();
        void setDroneDistance(float d);

        void setDronePresent(bool present);
        bool isDronePresent();

        bool isDroneInside();
        void setDroneInside(bool inside);

        void setDoorOpen(bool open);
        bool isDoorOpen();

        bool isInPreAlarm();
        void setPreAlarm(bool preAlarm);
        bool isInAlarm();
        void setAlarm(bool alarm);

        bool isTakeOffRequest();
        void setTakeOffRequest(bool req);
        bool isLandingRequest();
        void setLandingRequest(bool req);

        bool isInLanding();
        bool isInTakeOff();
        void setLanding(bool landing);
        void setTakeOff(bool takeoff);

        float getTemperature();

    private:
        AlarmState alarmState;

        bool dronePresent; 

        bool droneInside;  
        
        bool reqTakeOff;   
        bool reqLanding;  

        bool inPreAlarm;
        bool inAlarm;

        float currentTemp;
        float currentDistance;

        bool doorOpen;

        bool landing;
        bool takeOff;
    };
    
    #endif