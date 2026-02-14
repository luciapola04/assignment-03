#include "devices/Sonar.h"

#include "Arduino.h"

portMUX_TYPE sonarMux = portMUX_INITIALIZER_UNLOCKED;

Sonar::Sonar(int echoP, int trigP, long maxTime) : echoPin(echoP), trigPin(trigP), timeOut(maxTime){
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);  
  temperature = 20;
}

void Sonar::setTemperature(float temp){
  temperature = temp;
}
float Sonar::getSoundSpeed(){
  return 331.5 + 0.6*temperature;   
}

float Sonar::getDistance(){
    digitalWrite(trigPin,LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin,HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin,LOW);
    
    portENTER_CRITICAL(&sonarMux);
    
    long tUS = pulseIn(echoPin, HIGH, timeOut);
    
    portEXIT_CRITICAL(&sonarMux);

    if (tUS == 0) {
        return NO_OBJ_DETECTED;
    } else {
        float t = tUS / 1000.0 / 1000.0 / 2;
        float d = t*getSoundSpeed();
        return d;  
    }
}

