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

LoRaHandler *lora;

void setup() {
	// initialize serial monitor
	Serial.begin(115200);

	// initialize LoRa WAN device
	lora = new LoRaHandler();
	lora->initialize(0xAA, 0xBB);
}

void loop() {

}
