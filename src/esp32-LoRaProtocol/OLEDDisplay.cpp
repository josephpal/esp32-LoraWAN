#include "OLEDDisplay.h"

void OLEDDisplay::OLEDsetup() {
   display=new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  
  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display->begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}





OLEDDisplay::OLEDDisplay() {
  Serial.println("ctr1");
  displayNumber = 1;
  privateMessage = "default";

  OLEDsetup(); 
}

OLEDDisplay::OLEDDisplay(int displayNumber, String msg) {
  Serial.println("ctr2");
  this->displayNumber = displayNumber;

  OLEDsetup();
  privateMessage = msg;

  printMessage(privateMessage);
  
}
/*
OLEDDisplay::OLEDstart() {
//reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  
  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever 
}
*/

/*OLEDDisplay::OLEDtext(String text) {
 

}*/

OLEDDisplay::~OLEDDisplay() {
  Serial.println("dtr");
}

void OLEDDisplay::printMessage(String text) {
  /*Serial.print("printMessage [");
  Serial.print(displayNumber);
  Serial.println("]: " + text);*/
  OLEDclear();
  display->print(text);
  display->display();
}


void OLEDDisplay::OLEDclear() {
  display->clearDisplay();
  display->setTextColor(WHITE);
  display->setTextSize (2);
  display->setCursor(0,0);
  display->display();
}
