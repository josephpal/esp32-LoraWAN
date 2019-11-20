#ifndef OLEDDISPLAY_H
#define OLEDDISPLAY_H

#include "Arduino.h"
//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

class OLEDDisplay {
public:
    void OLEDsetup();
    OLEDDisplay();
    OLEDDisplay(int displayNumber, String msg);
    //OLEDtext(String text);
    void OLEDclear();
    
    ~OLEDDisplay();

    void printMessage(String text);

private:
  Adafruit_SSD1306 *display;
  String privateMessage;
  int displayNumber;

};

#endif
