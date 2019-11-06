#include "OLEDDisplay.h"

OLEDDisplay::OLEDDisplay() {
  Serial.println("ctr1");
  displayNumber = 1;
  privateMessage = "default";
}

OLEDDisplay::OLEDDisplay(int displayNumber, String msg) {
  Serial.println("ctr2");
  this->displayNumber = displayNumber;
  privateMessage = msg;

  printMessage(privateMessage);
}

OLEDDisplay::~OLEDDisplay() {
  Serial.println("dtr");
}

void OLEDDisplay::printMessage(String text) {
  Serial.print("printMessage [");
  Serial.print(displayNumber);
  Serial.println("]: " + text);
}
