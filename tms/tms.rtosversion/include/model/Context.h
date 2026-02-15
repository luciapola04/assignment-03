#include "HWPlatform.h"
#include <PubSubClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
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

        SemaphoreHandle_t mutex;

        template <typename T>
        void safeSet(T& variable, T value) {
            if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
                variable = value;
                xSemaphoreGive(mutex);
            }
        }

        template <typename T>
        T safeGet(T& variable) {
            T temp = {};
            if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
                temp = variable;
                xSemaphoreGive(mutex);
            }
            return temp;
        }
    };
    
    #endif