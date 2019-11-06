 /*
 * OledHandler.h
 *
 *  Created on: Nov 06, 2018
 *      Author: Panic S.
 *
 *  description:   
 */
   
#include "OledHandler.h"

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

static const unsigned char PROGMEM logo16_glcd_bmp[] = { 
  B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 
};

cOledHandler* cOledHandler::instance = 0;	/* statische Elemente einer Klasse m�ssen initialisiert werden. - Singleton voodoo*/

cOledHandler::cOledHandler()//:display(Adafruit_SSD1306(128,64))
{
	mOledPreBufferPtr = new oledPreBuffer{ false, false, DT_Empty, "", "", "", "", 0, NULL };
	
	xTaskCreatePinnedToCore
	(
		printDisplay,				// Function to implement the task
		"initQueue_static",			// Name of the task
		10000,						// Stack size in words
		(void*)mOledPreBufferPtr,	// Task input parameter
		0,							// Priority of the task
		NULL,						// Task handle.
		0							// Core where the task should run
	);
}

cOledHandler::~cOledHandler()
{
	//delete display;
}

cOledHandler* cOledHandler::getInstance()
{
	if (!instance)
		instance = new cOledHandler();
	return instance;
}

void cOledHandler::printDisplay(void * arg)
{
	oledPreBuffer* mOledPreBufferPtr = (oledPreBuffer*)arg;
	//Setup of display
	Adafruit_SSD1306 display_intern(128, 64);
	display_intern.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	display_intern.clearDisplay();

	unsigned long starttime, endtime, runtime;

	while (true)
	{
		starttime = millis();
		if (false == mOledPreBufferPtr->bufMutexLocked)	//check if mutex is not taken, else skip
		{
			mOledPreBufferPtr->bufMutexLocked = true;	//take mutex - writing to buffer is blocked
			if (mOledPreBufferPtr->bufNewScreen)	//check if anything to display, else skip
			{
				display_intern.clearDisplay();
				display_intern.setTextSize(1);
				display_intern.setTextColor(WHITE);
				display_intern.setCursor(0, 0);

				/* Switch between differen Display types, e.g.:
				simple message, connectionState (ak main screen), QueueState, etc.
				Print display accordingly
				*/
				switch (mOledPreBufferPtr->mDisplayType)
				{
				case DT_Message:	//simple text message, printed on top of display
					display_intern.println(mOledPreBufferPtr->bufMessage);
					break;
				case DT_ConState:	//network information
					if (0 == mOledPreBufferPtr->bufMode)
					{
						display_intern.println("Connected to network");
					}
					else {
						display_intern.println("Access point created");
					}

					display_intern.println("--------------------");
					display_intern.println("Network name:");
					display_intern.println(mOledPreBufferPtr->bufnetworkName);
					display_intern.println();
					display_intern.println("FT32 IP address:");
					display_intern.println(mOledPreBufferPtr->bufIPaddress);
					display_intern.println("--------------------");
					break;
				case DT_ConStatePW:	//network information with password (only recomended, if accesspoint created)
					if (0 == mOledPreBufferPtr->bufMode)
					{
						display_intern.println("Connected to network");
					}
					else {
						display_intern.println("Access point created");
					}

					display_intern.println("--------------------");
					display_intern.println("Network name:");
					display_intern.println(mOledPreBufferPtr->bufnetworkName);
					display_intern.println("PW:" + mOledPreBufferPtr->bufPassword);
					display_intern.println("FT32 IP address:");
					display_intern.println(mOledPreBufferPtr->bufIPaddress);
					display_intern.println("--------------------");
					break;
				case DT_QueState:	//print run-time-informations on running program
					
					for (int i = 0; i < 8; i++)	//print digital ButtonStates
					{
						display_intern.setCursor((i * 15 + 1), 1);
						if ((bool)mOledPreBufferPtr->bufSHMptr->digitalVal[i])
						{
							display_intern.fillRect(i * 15, 0, 13, 9, WHITE);
							display_intern.setTextColor(BLACK);
						}
						else
						{
							display_intern.setTextColor(WHITE);
						}
						display_intern.print("T" + (String)i);// + (String)((bool)mSHMptr->digitalVal[i]));
						display_intern.setCursor((i * 15 + 1), 9);
						display_intern.setTextColor(WHITE);
						display_intern.print(mOledPreBufferPtr->bufSHMptr->analogVal[i]/2);
					}

					for (int i = 0; i < 4; i++)	//print motor values
					{
						display_intern.setCursor((i * 30 + 1), 17);
						display_intern.print("M" + (String)i + ":" + (String)mOledPreBufferPtr->bufSHMptr->motorVal[i]);
					}

					display_intern.setCursor(1, 25);	//print servo values
					display_intern.print("S" + (String)0 + ":" + (String)mOledPreBufferPtr->bufSHMptr->servoVal[0]);
				default:
					break;
				}

				display_intern.display();
				mOledPreBufferPtr->mDisplayType = DT_Empty;
				mOledPreBufferPtr->bufNewScreen = false;
			}
			mOledPreBufferPtr->bufMutexLocked = false;
		}
		
		endtime = millis();
		runtime = endtime - starttime;
		//Serial.println("[oled_main] Endtime: " + (String)endtime + " Diff: " + (String)(runtime));
		runtime = runtime < 199 ? runtime : 198;	//limit runtime to max. 200ms for delay();
		delay(199 - runtime);	//ensures a ~5Hz refresh rate for display
	}

	vTaskDelete(NULL);
}

int cOledHandler::printLoginData(String pIP, String pPasswort, String pSsid, int mode) {//Adafruit_SSD1306 &display, String pIP, String pPasswort, String pssid) {
	if (!mOledPreBufferPtr->bufMutexLocked)
	{
		mOledPreBufferPtr->bufMutexLocked = true;
		mOledPreBufferPtr->mDisplayType = DT_ConStatePW;
		mOledPreBufferPtr->bufIPaddress = pIP;
		mOledPreBufferPtr->bufPassword = pPasswort;
		mOledPreBufferPtr->bufnetworkName = pSsid;
		mOledPreBufferPtr->bufMode = mode;
		mOledPreBufferPtr->bufNewScreen = true;
		mOledPreBufferPtr->bufMutexLocked = false;
		
		return 1;
	}
	else
	{
		return 0;
	}
}

int cOledHandler::printConnectionStatus(String pIP, String pSsid, int mode){//Adafruit_SSD1306 &display, String pIP, String pSsid, int mode) {
	if (!mOledPreBufferPtr->bufMutexLocked)
	{
		mOledPreBufferPtr->bufMutexLocked = true;
		mOledPreBufferPtr->mDisplayType = DT_ConState;
		mOledPreBufferPtr->bufIPaddress = pIP;
		mOledPreBufferPtr->bufnetworkName = pSsid;
		mOledPreBufferPtr->bufMode = mode;
		mOledPreBufferPtr->bufNewScreen = true;
		mOledPreBufferPtr->bufMutexLocked = false;

		return 1;
	}
	else
	{
		return 0;
	}
}

int cOledHandler::printOledMessage(String message) {//Adafruit_SSD1306 &display, String message) {
	if (!mOledPreBufferPtr->bufMutexLocked)
	{
		mOledPreBufferPtr->bufMutexLocked = true;
		mOledPreBufferPtr->mDisplayType = DT_Message;
		mOledPreBufferPtr->bufMessage = message;
		mOledPreBufferPtr->bufNewScreen = true;
		mOledPreBufferPtr->bufMutexLocked = false;

		return 1;
	}
	else
	{
		return 0;
	}
}

void cOledHandler::printQueueWindow(void* shm_ptr)
{
	if (!mOledPreBufferPtr->bufMutexLocked)
	{
		mOledPreBufferPtr->bufMutexLocked = true;
		mOledPreBufferPtr->mDisplayType = DT_QueState;
		mOledPreBufferPtr->bufSHMptr = (SHM*)shm_ptr;
		mOledPreBufferPtr->bufNewScreen = true;
		mOledPreBufferPtr->bufMutexLocked = false;
	}
}
