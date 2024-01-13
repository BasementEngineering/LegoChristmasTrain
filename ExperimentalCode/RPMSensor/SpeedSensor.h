#ifndef SPEED_SENSOR
#define SPEED_SENSOR

#define BUFFER_SIZE 6

class SpeedSensor{
  private:
  int myPin;
  unsigned long lastUpdate;
  
  bool lastState;
  unsigned long lastRoundSignal;
  unsigned long durationWindow[BUFFER_SIZE];
  int slotCounter;
  
  public:
  SpeedSensor();  
  void begin(int pin);
  void update();
  void resetBuffer();
  unsigned long getAvgDuration();
  float getSpeed();
};
#endif
