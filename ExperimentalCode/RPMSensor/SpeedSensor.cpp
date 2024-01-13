#include "SpeedSensor.h"
#include "Arduino.h"

SpeedSensor::SpeedSensor(){
    lastUpdate = 0;
    lastState = false;
    lastRoundSignal = 0;
    slotCounter = 0;
    resetBuffer();
  }
  
  void SpeedSensor::begin(int pin){
    myPin = pin;
    pinMode(myPin,INPUT_PULLUP);
  }

  void SpeedSensor::update(){
    unsigned long currentTime = millis();
    if(currentTime - lastUpdate > 10){
    lastUpdate = currentTime;
    bool state = digitalRead(myPin);
    if(state && !lastState){
      durationWindow[slotCounter] = currentTime - lastRoundSignal;
      slotCounter++;
      slotCounter%=BUFFER_SIZE;
      lastRoundSignal = currentTime;
    }

    if(currentTime - lastRoundSignal > 1000){
      resetBuffer();
    }
    lastState = state;
  }
  }

  void SpeedSensor::resetBuffer(){
    for(int i = 0; i < BUFFER_SIZE; i++){
      durationWindow[i] = 0;
    }
  }
  
  unsigned long SpeedSensor::getAvgDuration(){
    unsigned long sum = 0;
    for(int i = 0; i < BUFFER_SIZE; i++){
      sum += durationWindow[i];
    }
    return sum/BUFFER_SIZE;
  }

  float SpeedSensor::getSpeed(){
    unsigned long avgDuration = getAvgDuration();
    if(avgDuration == 0){
      return 0.0;
    }
    //Diameter = 16mm -> circumference = 50,27mm aka. 0.05027 m
    float distancePerRevolution = 0.05027;
    return 3.6 * (distancePerRevolution/(avgDuration*0.001));
  }
