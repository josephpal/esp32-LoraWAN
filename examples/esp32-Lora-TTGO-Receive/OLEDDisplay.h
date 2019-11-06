#ifndef OLEDDISPLAY_H
#define OLEDDISPLAY_H

#include "Arduino.h"

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

class OLEDDisplay {
public:
    OLEDDisplay();
    OLEDDisplay(int displayNumber, String msg);
    ~OLEDDisplay();

    void printMessage(String text);

private:
  String privateMessage;
  int displayNumber;

};

#endif
