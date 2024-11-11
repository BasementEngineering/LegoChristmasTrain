#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

#include <Wire.h>
#include <AS5600.h>
#include <Arduino.h>

#define SPEEDOMETER_UPDATE_INTERVAL_MS 3

#define SENSE_INTERVAL 2

const float circumference_m = 0.05027;

static volatile float currentSpeed = 0.0;
static volatile int newAngle = 0;
static volatile bool newReading = false;
static volatile int oldAngle = 0;
static volatile int revolutionDelta = 0;
static int analogSensePin;

void IRAM_ATTR getReading() {
      newAngle = analogRead(analogSensePin);
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
        newSpeed = angularVelocity * (circumference_m / 4095);
        
      }

      currentSpeed = newSpeed;//(newSpeed + lastSpeed) * 0.5;
      oldAngle = newAngle;
      //lastSpeed = currentSpeed;
    }

hw_timer_t *Timer0_Cfg = NULL;

void setupTimerInterrupt(){
  Timer0_Cfg = timerBegin(1000000);
  timerAttachInterrupt(Timer0_Cfg, &getReading);
  timerAlarm(Timer0_Cfg, SPEEDOMETER_UPDATE_INTERVAL_MS * 1000, true, 0);
}

class Speedometer {
  private:
    AMS_5600 ams5600;
    unsigned long lastUpdate = 0;

    bool inverted = false;  

  public:
    Speedometer(int analogPin) {
      analogSensePin = analogPin;
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
      pinMode(analogSensePin, INPUT);
    }

    float convertRawAngleToDegrees(word newAngle)
    {
      /* Raw data reports 0 - 4095 segments, which is 0.087890625 of a degree */
      float retVal = newAngle * 0.087890625;
      return retVal;
    }

    float getSpeed() {
      return 0.0;//currentSpeed;
    }

    float getDistance() {
      return 0.0;//revolutionDelta * circumference_m;
    }

    void update() {
      if(millis() - lastUpdate > 100){

        Serial.print("speed: " + String(currentSpeed));
        Serial.println();
        lastUpdate = millis();
      }
    }
};

#endif
