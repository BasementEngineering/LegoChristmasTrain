#include "Speedometer.h"
#include "SpeedMonitor.h"
#include "TrainMotor.h"


#define MOTOR_PIN 12
#define ANALOG_SENSOR_PIN 32
#define SPEED_POTI_PIN 34

SpeedMonitor speedMonitor;
Speedometer speedometer(ANALOG_SENSOR_PIN);
TrainMotor motor(MOTOR_PIN);

hw_timer_t *Timer0_Cfg = NULL;
int counter = 0;

void IRAM_ATTR Timer0_ISR()
{
  speedometer.getReading();
}

void setupTimerInterrupt(){
  Timer0_Cfg = timerBegin(1000000);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR);
  timerAlarm(Timer0_Cfg, speedometer.getInterval() * 1000, true, 0);
}

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
  delay(1000);
  Serial.println("Starting setup");
  delay(1000);
  Serial.println("Setting up speed monitor");
  speedMonitor.begin();
  speedMonitor.setSpeed(40);
  delay(1000);
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
  speedometer.update();
  speedMonitor.setSpeed(speedometer.getSpeed());
  speedMonitor.setDistance(speedometer.getDistance());
  //speedMonitor.update();
  updateMotor();
}
