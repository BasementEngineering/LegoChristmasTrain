#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Speedometer.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


class SpeedMonitor{

  public:
  SpeedMonitor(){
    lastUpdate = 0;
  }

  void setSpeed(float speed){
    this->speed = speed;
  }

  void setDistance(float distance){
    this->distance = distance;
  }

  void update(){
    if(millis() - lastUpdate > 200){
      draw();
      lastUpdate = millis();
    }
  }

  void draw(){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Speed: ");
    display.setTextSize(2);
    display.print(this->speed);
    display.setTextSize(1);
    display.println(" m/s");
    display.setCursor(0, 16);
    display.print("Distance: ");
    display.setTextSize(2);
    display.print(this->distance);
    display.setTextSize(1);
    display.println(" m");
    display.display();
  }

  void begin(){
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.display();
    delay(2000);
    display.clearDisplay();
    draw();
  }

  private:
    float speed;
    float distance;
    unsigned long lastUpdate;
};

SpeedMonitor speedMonitor;
Speedometer speedometer;

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
  Serial.println("Ready to go");
}

void loop() {
  speedometer.update();
  speedMonitor.setSpeed(speedometer.getSpeed());
  speedMonitor.setDistance(speedometer.getDistance());
  speedMonitor.update();
}