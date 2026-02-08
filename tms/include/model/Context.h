#include "HWPlatform.h"
#include <PubSubClient.h>
#ifndef __CONTEXT__
#define __CONTEXT__

class Context {
    public:
        Context();
        void init();

        void setSystemOnline(bool status);
        bool isSystemOnline();

        void setMQTTClient(PubSubClient* client);
        PubSubClient* getMQTTClient();

        void setMqttError(bool status);
        bool isMqttError();

    private:
        bool online;
        PubSubClient* mqttClient;
        bool mqttError;
    };
    
    #endif