#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

#include <Wire.h>
#include <AS5600.h>
#include <Arduino.h>

#define SPEEDOMETER_UPDATE_INTERVAL_MS 3

#define SENSE_INTERVAL 2

class Speedometer {
  private:
    AMS_5600 ams5600;
    int sensorStartAngle = 0;
    int oldAngle = 0;
    int revolutionDelta = 0;

    unsigned long lastUpdate = 0;

    bool inverted = false;

    float circumference_m = 0.05027;

    float currentSpeed = 0.0;
    //float lastSpeed = 0.0;

    int newAngle = 0;
    bool newReading = false;
    int analogPin;

  public:
    Speedometer(int analogPin) {
      this->analogPin = analogPin;
    }

    int getInterval() {
      return SENSE_INTERVAL;
    }

    void setupEncoder() {
      Wire.begin(); //SDA, SCL
      Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>> ");
      if (ams5600.detectMagnet() == 0 ) {
        while (1) {
          if (ams5600.detectMagnet() == 1 ) {
            Serial.print("Current Magnitude: ");
            Serial.println(ams5600.getMagnitude());
            ams5600.setOutPut(1);
            break;
          }
          else {
            Serial.println("Can not detect magnet");
          }
          delay(1000);
        }
      }
      pinMode(analogPin, INPUT);
    }

    float convertRawAngleToDegrees(word newAngle)
    {
      /* Raw data reports 0 - 4095 segments, which is 0.087890625 of a degree */
      float retVal = newAngle * 0.087890625;
      return retVal;
    }

    float getDistance() {
      return revolutionDelta * circumference_m;
    }

    void setSensorStartAngle() {
      sensorStartAngle = getAngle();
      oldAngle = sensorStartAngle;
      revolutionDelta = 0;
    }

    int detectRollovers(int newAngle, int oldAngle) {
      bool rollUnder = (oldAngle < 90) && (newAngle > 270);
      bool rollOver = (oldAngle > 270) && (newAngle < 90);
      if ( rollOver ) {
        //Serial.println("Rollover detected");
        revolutionDelta ++;
        return 1;
      }
      else if ( rollUnder ) {
        //Serial.println("Rollunder detected");
        revolutionDelta --;
        return -1;
      }
      return 0;
    }

    int getAngleDelta(int newAngle, int oldAngle, int rolloverFactor) {
      int delta = 0;
      if (rolloverFactor == 0) {
        delta = newAngle - oldAngle;
      }
      else if (rolloverFactor == 1) {
        delta = newAngle + (360 - oldAngle);
      }
      else if (rolloverFactor == -1) {
        delta = (360 - newAngle) + oldAngle;
      }

      return delta;
    }

    void IRAM_ATTR getReading() {
      newAngle = analogRead(analogPin);
      newAngle = map(newAngle, 0, 4095, 0, 360);
      float newSpeed = 0.0;

      if(newAngle != oldAngle){
        int angleDelta = 0;
        float angularVelocity = 0.0;

        if((oldAngle < 150) && (newAngle > 210)){ //Rollunder
            angleDelta = (360 - newAngle) + oldAngle;
            revolutionDelta --;
        }
        else if((oldAngle > 210) && (newAngle < 150)){ //Rollover
            angleDelta = newAngle + (360 - oldAngle);
            revolutionDelta ++;
        }
        else{
            angleDelta = newAngle - oldAngle;
        }

        angularVelocity = angleDelta * 1000 / SENSE_INTERVAL;
        newSpeed = angularVelocity * (circumference_m / 360.0);
        
      }

      currentSpeed = newSpeed;//(newSpeed + lastSpeed) * 0.5;
      oldAngle = newAngle;
      //lastSpeed = currentSpeed;
    }

    void update() {
      if(millis() - lastUpdate > 100){

        Serial.print("speed: " + String(currentSpeed));
        Serial.println();
        lastUpdate = millis();
      }
    }

    int getAngle() {
      int angle = ams5600.getRawAngle();
      if (!inverted) {
        return convertRawAngleToDegrees(angle);
      }
      else {
        return 360 - convertRawAngleToDegrees(angle);
      }
    }

    float getSpeed() {
      return currentSpeed;
    }
};

#endif
