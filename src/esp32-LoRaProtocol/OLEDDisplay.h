#ifndef OLEDDISPLAY_H
#define OLEDDISPLAY_H

#include "Arduino.h"
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
    OLEDDisplay();

    ~OLEDDisplay();

    /**
     *
    */
    void OLEDsetup();

    /**
     * Displays text in the next line in the given size.
     * If the text needs more space, that it is left on
     * the display, the display will be cleared and the
     * text will be displayed in the first line
	*/
    void OLEDtext(String text, int textsize);

    /**
     * Displays text in the next line in the size, that
     * was set in the last comand, that was made with
     * the size, if no size was ever set, the default size is 1.
     * If the text needs more space, that it is left on
     * the display, the display will be cleared and the
     * text will be displayed in the first line
    */
    void OLEDtext(String text);

    /**
     * Clears the displayand displays the text in the first
     * line in the given size.
    */
    void OLEDnewtext(String text, int textsize);

    /**
     * Clears the displayand displays the text in the first
     * line in the size, that was set in the last comand, that
     * was made with the size, if no size was ever set, the
     * default size is 1.
    */
    void OLEDnewtext(String text);

    /**
     * Displays the text in the last line(s, if it needs more
     * than one line) and the last texts above, so it looks
     * like one is scolling through the text.
    */
    void OLEDfloattext(String text, int textsize);

    /**
     * Clears the display and set the cursor to the first line.
    */
    void OLEDclear();

private:
  /* functions */
  void cal_linenumber(String text,int textsize);

  /* Variables */
  int tsize;
  int linenumber;
  int linecounter;

  String texts[8];
  int textsizes[8];

  int nextline;
  int i;
  int textlength;
  int line;
  int lines[8];

  Adafruit_SSD1306 *display;
};

#endif
