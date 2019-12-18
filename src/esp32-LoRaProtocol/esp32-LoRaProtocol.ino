/*
 * esp32-LoRaProtocol.ino
 *
 *  Created on: 	Nov 13, 2019
 *  				joseph
 *  
 *  description:
 *  
 *  
 *  notifications:
 *  
 */

#include "LoRaHandler.h"
#include "OLEDDisplay.h"

#define SENDER

/*
 * 	#ifdef SENDER_ESP32
		// sender module ESP32 DEV module with top shield (LoRaWAN + GPS)
		lora->setSPIPins(18, 19, 23, 5);
		lora->setLORAPins(5, 16, 2);

		lora->initialize(0xAA, 0xBB);
		lora->setTxPower(17);

		lora->initEncryption("abcdefghijklmnop");

		lora->startReceiveListner();
	#endif
 */

LoRaHandler *lora;
char *encryptionKey = "abcdefghijklmnop";

void setup() {
	// initialize serial monitor
	Serial.begin(115200);

	// initialize LoRa WAN device
	lora = new LoRaHandler();

	// module TTGO ESP32
	#ifdef SENDER
		lora->initialize(0xAA, 0xBB);
	#else
		lora->initialize(0xBB, 0xAA);
	#endif

	lora->setTxPower(17);

	lora->initEncryption(encryptionKey);

	lora->startReceiveListner();
}

void loop() {
	#ifdef SENDER
		delay(500);
		lora->send("broadcast", 0xFF);
		delay(500);
		lora->send("local");
	#endif
}
