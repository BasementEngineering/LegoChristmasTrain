#include "Speedometer.h"
#include "SpeedMonitor.h"
#include "TrainMotor.h"


#define MOTOR_PIN 26
#define ANALOG_SENSOR_PIN 32
#define SPEED_POTI_PIN 34

SpeedMonitor speedMonitor;
Speedometer speedometer(ANALOG_SENSOR_PIN);
TrainMotor motor(MOTOR_PIN);

unsigned long lastMotorSpeedUpdate = 0;

void updateMotor(){
  if( (millis() - lastMotorSpeedUpdate) > 100){
    int speed = map(analogRead(SPEED_POTI_PIN), 0, 4095, -100, 100);
  if(speed < 10 && speed > -10){
    speed = 0;
  }
  Serial.println(speed);
  motor.setSpeed(speed);
  lastMotorSpeedUpdate = millis();
  }
  
}

void setup() {
  Serial.begin(9600);
  delay(500);
  Serial.println("Starting setup");
  delay(500);
  Serial.println("Setting up speed monitor");
  speedMonitor.begin();
  
  speedMonitor.setSpeed(40);
  speedMonitor.update();
  delay(500);
  Serial.println("Setting up encoder");
  speedometer.setupEncoder();
  Serial.println("Setting up motor");
  motor.begin();
  Serial.println("Activating HW Timer");
  setupTimerInterrupt();
  Serial.println("Ready to go");
  pinMode(SPEED_POTI_PIN, INPUT);
}

void loop() {
  delay(10);
  speedometer.update();
  speedMonitor.setSpeed(speedometer.getSpeed());
  speedMonitor.setDistance(speedometer.getDistance());
  speedMonitor.update();
  updateMotor();
}
