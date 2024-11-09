#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

#include <Wire.h>
#include <AS5600.h>
#include <Arduino.h>

#define SPEEDOMETER_UPDATE_INTERVAL_MS 3

class Speedometer {
private:
    AMS_5600 ams5600;
    int sensorStartAngle = 0;
    int previousSensorAngle = 0;
    int revolutionDelta = 0;

    unsigned long lastUpdate = 0;

    bool inverted = false;

    float circumference_m = 0.05027;

    float currentSpeed = 0.0;

public:
    void setupEncoder(){
        Wire.begin(); //SDA, SCL
        Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>> ");
        if(ams5600.detectMagnet() == 0 ){
            while(1){
                    if(ams5600.detectMagnet() == 1 ){
                            Serial.print("Current Magnitude: ");
                            Serial.println(ams5600.getMagnitude());
                            break;
                    }
                    else{
                            Serial.println("Can not detect magnet");
                    }
                    delay(1000);
            }
        }
    }

    float convertRawAngleToDegrees(word newAngle)
    {
        /* Raw data reports 0 - 4095 segments, which is 0.087890625 of a degree */
        float retVal = newAngle * 0.087890625;
        return retVal;
    }

    float getDistance(){
        return revolutionDelta * circumference_m;
    }

    void setSensorStartAngle(){
        sensorStartAngle = getAngle();
        previousSensorAngle = sensorStartAngle;
        revolutionDelta = 0;
    }

    int detectRollovers(int newAngle, int oldAngle){
        bool rollUnder = (oldAngle < 90) && (newAngle > 270);
        bool rollOver = (oldAngle > 270) && (newAngle < 90);
        if( rollOver ){
            Serial.println("Rollover detected");
            revolutionDelta ++;
            return 1;
        }
        else if( rollUnder ){
            Serial.println("Rollunder detected");
            revolutionDelta --;
            return -1;
        }
        return 0;
    }

    int getAngleDelta(int newAngle, int oldAngle, int rolloverFactor){
        int delta = 0;
        if(rolloverFactor == 0){
            delta = newAngle - oldAngle;
        }
        else if(rolloverFactor == 1){
            delta = newAngle + (360 - oldAngle);
        }
        else if(rolloverFactor == -1){
            delta = (360 - newAngle) + oldAngle;
        }
        
        return delta;
    }

    void update(){
        if(millis() - lastUpdate > SPEEDOMETER_UPDATE_INTERVAL_MS){
          int interval = millis() - lastUpdate;
            int newAngle = getAngle();
            float angularVelocity = 0.0;
            int angleDelta = 0;
            if(newAngle != previousSensorAngle){
                int rolloverFactor = detectRollovers(newAngle,previousSensorAngle);
                angleDelta = getAngleDelta(newAngle,previousSensorAngle,rolloverFactor);

                angularVelocity = angleDelta * 1000 / interval;
                currentSpeed = angularVelocity * (circumference_m / 360.0);

                
                
                previousSensorAngle = newAngle;
            }
            else{
              currentSpeed = 0.0;
            }

            Serial.print("Delta: "+ String(angleDelta) + ",");
                Serial.print("angularVelocity: "+ String(angularVelocity)+ ",");
                Serial.print("speed: "+ String(currentSpeed)+ ",");
                Serial.println("Interval: "+ String(millis() - lastUpdate));
            lastUpdate = millis();
        }
    }

    int getAngle(){
        int angle = ams5600.getRawAngle();
        if(!inverted){
          return convertRawAngleToDegrees(angle);
        }
        else{
          return 360 - convertRawAngleToDegrees(angle);
        }
    }

    float getSpeed(){
        return currentSpeed;
    }
};

#endif