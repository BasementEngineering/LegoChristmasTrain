#include "Speedometer.h"
#include "SpeedMonitor.h"

#define ANALOG_SENSOR_PIN 32

SpeedMonitor speedMonitor;
Speedometer speedometer(ANALOG_SENSOR_PIN);

hw_timer_t *Timer0_Cfg = NULL;
int counter = 0;

void IRAM_ATTR Timer0_ISR()
{
  speedometer.getReading();
}

void setupTimerInterrupt(){
  //IPC0bits.T1IP = 0b001;
  
  Timer0_Cfg = timerBegin(1000000);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR);
  timerAlarm(Timer0_Cfg, speedometer.getInterval() * 1000, true, 0);
  //timerAlarmEnable(Timer0_Cfg);
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
  Serial.println("Activating HW Timer");
  setupTimerInterrupt();
  Serial.println("Ready to go");
}

void loop() {
  speedometer.update();
  speedMonitor.setSpeed(speedometer.getSpeed());
  speedMonitor.setDistance(speedometer.getDistance());
}
