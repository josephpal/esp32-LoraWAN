/*
 * OledHandler.h
 *
 *  Created on: Nov 06, 2018
 *      Author: Panic S.
 *
 *  description:   
 */

#ifndef OLEDHANDLER_H
#define OLEDHANDLER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "ft_ESP32_SHM.h"

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

enum Display_Type
{
	DT_Empty,
	DT_Message,
	DT_ConState,
	DT_ConStatePW,
	DT_QueState,
	DT_ScreenSav
};

struct oledPreBuffer
{
	bool bufMutexLocked;
	bool bufNewScreen;
	Display_Type mDisplayType;
	String bufMessage;
	String bufIPaddress;
	String bufnetworkName;
	String bufPassword;
	int bufMode;
	SHM* bufSHMptr;
};

/* Display-Class, singleton, for handling any display-relevant stuff of FT32
*  based on: https://de.wikibooks.org/wiki/C%2B%2B-Programmierung:_Entwurfsmuster:_Singleton
*/

class cOledHandler
{
public:
	static cOledHandler* getInstance();

	/* get the ip address of the server
	*  and print on Oled e.g.: 192.168.4.1
	*   
	*	@param IP-address as string (xxx.xxx.xxx.xxx)
	*	@param password as string in clear text (not recomended...)
	*	@param ssid (network name) as string
	*	@param mode=0 if connected to network, else if created own access point
	*	@return 1: print will be done, 0: it will not be printed, mutex was taken
	*/
	int printLoginData(String pIP, String pPasswort, String pSsid, int mode);

	/* display Verbunden after login
	*
	*	@param IP-address as string (xxx.xxx.xxx.xxx)
	*	@param ssid (network name) as string
	*	@param mode=0 if connected to network, else if created own access point
	*	@return 1: print will be done, 0: it will not be printed, mutex was taken
	*/
	int printConnectionStatus(String pIP, String pSsid, int mode);

	/* display any given message
	*
	*	@param message to be displayed
	*	@return 1: print will be done, 0: it will not be printed, mutex was taken
	*/
	int printOledMessage(String message = "");

	/*	Print basic program and state information on window while running
	*	
	*	@param pointer on SHM (shared memory) (will be cast to SHM* internally)
	*	@return ---
	*/
	void printQueueWindow(void* shm_ptr);

private:
	static cOledHandler* instance;

	/* verhindert, dass ein Objekt von au�erhalb von N erzeugt wird.
	 initialises oled-display
	*/
	cOledHandler();

	/* verhindert, dass eine weitere Instanz via Kopie-Konstruktor erstellt werden kann */
	cOledHandler(const cOledHandler&);

	~cOledHandler();
	
	static void printDisplay(void* arg);

	oledPreBuffer* mOledPreBufferPtr;
};



#endif
