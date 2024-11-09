#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <Servo.h>

class Motor{
  public:
  Motor(int driverPin):motorDriver(){
    driverPin = driverPin;
  }

  void begin(){
    motorDriver.attach(driverPin);
    motorDriver.writeMicroseconds(1500);
  }

  void setSpeed(int speed){
    int mircorsenconds = map(speed, -100, 100, 1000, 2000);
    Serial.println(mircorsenconds);
  }

  private:
  int driverPin;
  Servo motorDriver;
};

#endif