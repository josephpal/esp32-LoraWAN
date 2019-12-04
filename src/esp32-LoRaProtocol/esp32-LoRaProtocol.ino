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

//#define SENDER_ESP32
#define RECEIVER

LoRaHandler *lora;

void setup() {
	// initialize serial monitor
	Serial.begin(115200);

	// initialize LoRa WAN device
	lora = new LoRaHandler();

	#ifdef SENDER_ESP32
		// sender module ESP32 DEV module with top shield (LoRaWAN + GPS)
		lora->setSPIPins(18, 19, 23, 5);
		lora->setLORAPins(5, 16, 2);

		lora->initialize(0xAA, 0xBB);
		SENSENDER_ESP32SENDER_ESP32DER_ESP32
		lora->setTxPower(17);

		lora->initEncryption("abcdefghijklmnop");

		lora->startReceiveListner();
	#else
		// receive module TTGO ESP32
		lora->initialize(0xBB, 0xAA);

		lora->setTxPower(17);

		lora->initEncryption("abcdefghijklmnop");

		lora->startReceiveListner();
	#endif
}

void loop() {
	#ifdef SENDER_ESP32
		delay(500);
		lora->send("Broadcast call", 0xFF);
		delay(500);
		lora->send("local call");
	#endif
}
