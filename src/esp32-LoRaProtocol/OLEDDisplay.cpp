#include "OLEDDisplay.h"

OLEDDisplay::OLEDDisplay() {
	OLEDsetup();
}

OLEDDisplay::~OLEDDisplay() {

}

void OLEDDisplay::OLEDsetup() {
	display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire,
			OLED_RST);

	//reset OLED display via software
	pinMode(OLED_RST, OUTPUT);
	digitalWrite(OLED_RST, LOW);
	delay(20);
	digitalWrite(OLED_RST, HIGH);

	//initialize OLED
	Wire.begin(OLED_SDA, OLED_SCL);
	if (!display->begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
		Serial.println(F("SSD1306 allocation failed"));
		for (;;)
			; // Don't proceed, loop forever
	}

	tsize = 1;
	linenumber = 0;
	linecounter = 0;

	display->clearDisplay();
	display->display();
}

void OLEDDisplay::OLEDtext(String text, int textsize) {
	tsize = textsize;
	cal_linenumber(text, textsize);
	nextline = linenumber + line * tsize * 8;

	if (nextline > 64) {         //control that the text it completly visible
		display->clearDisplay();
		display->display();
		linenumber = 0;
	}

	display->setTextColor(WHITE);
	display->setTextSize(tsize);
	display->setCursor(0, linenumber);
	display->print(text);
	display->display();
	linenumber = linenumber + line * tsize * 8;
}

void OLEDDisplay::OLEDtext(String text) {
	cal_linenumber(text, tsize);
	nextline = linenumber + line * tsize * 8;

	if (nextline > 64) {         //control that the text it completly visible
		display->clearDisplay();
		display->display();
		linenumber = 0;
	}

	display->setTextColor(WHITE);
	display->setTextSize(tsize);
	display->setCursor(0, linenumber);
	display->print(text);
	display->display();
	linenumber = linenumber + line * tsize * 8;
}

void OLEDDisplay::OLEDnewtext(String text, int textsize) {
	tsize = textsize;
	display->clearDisplay();
	display->display();

	display->setTextColor(WHITE);
	display->setTextSize(tsize);
	display->setCursor(0, 0);
	display->print(text);
	display->display();
	cal_linenumber(text, tsize);
	linenumber = line * tsize * 8;
}

void OLEDDisplay::OLEDnewtext(String text) {
	display->clearDisplay();
	display->display();

	display->setTextColor(WHITE);
	display->setTextSize(tsize);
	display->setCursor(0, 0);
	display->print(text);
	display->display();
	cal_linenumber(text, tsize);
	linenumber = line * tsize * 8;
}

void OLEDDisplay::OLEDfloattext(String text, int textsize) {

	if (linecounter >= 7) { //when linecounter=7 the last (8th) line at size 1 is choosen, because the first line is indicated with 0
		linecounter = 7;
		linenumber = 64;
		cal_linenumber(text, textsize);
		//update arrays
		texts[7] = text;
		textsizes[7] = textsize;
		lines[7] = line;
		display->clearDisplay();

		for (i = 7; i >= 0; i--) {
			linenumber = linenumber - lines[i] * textsizes[i] * 8;

			display->setTextSize(textsizes[i]);
			display->setCursor(0, linenumber);
			display->print(texts[i]);

			//further textlines that will not be dispalyed will not be calculated and configured
			if (linenumber < 0) {
				break;
			}
		}

		display->display();

		//shifting the information
		for (i = 0; i < 7; i++) {
			texts[i] = texts[i + 1];
			textsizes[i] = textsizes[i + 1];
			lines[i] = lines[i + 1];
		}
	} else {
		//print text and fill the array

		OLEDtext(text, textsize);
		cal_linenumber(text, textsize);
		texts[linecounter] = text;
		textsizes[linecounter] = textsize;
		lines[linecounter] = line;
	}

	linecounter = linecounter + 1;
}

void OLEDDisplay::OLEDclear() {
	linenumber = 0;
	display->clearDisplay();
	display->setTextColor(WHITE);
	display->setTextSize(tsize);
	display->setCursor(0, linenumber);
	display->display();
}

void OLEDDisplay::cal_linenumber(String text, int textsize) {
	textlength = text.length();
	switch (textsize) {
	case 1:
		line = textlength / 21 + 1;
		break;
	case 2:
		line = textlength / 10 + 1;
		break;
	case 3:
		line = textlength / 7 + 1;
		break;
	case 4:
		line = textlength / 5 + 1;
		break;
	case 5:
		line = textlength / 4 + 1;
		break;
	case 6:
		line = textlength / 3 + 1;
		break;
	case 7:
		line = textlength / 3 + 1;
		break;
	case 8:
		line = textlength / 2 + 1;
		break;
	}

}
