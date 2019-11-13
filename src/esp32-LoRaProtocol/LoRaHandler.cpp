/*
 * LoRaHandler.cpp
 *
 *  Created on: Nov 13, 2019
 *      Author: joseph
 */

#include "LoRaHandler.h"

LoRaHandler::LoRaHandler() {
	if(!Serial) {
		Serial.begin(115200);
	}

	outgoing = "";

	msgCount = 0;
	localAddress = 0xBB;
	destination = 0xFF;
}

LoRaHandler::~LoRaHandler() {
	// TODO Auto-generated destructor stub
}

void LoRaHandler::initialize() {
	//SPI LoRa pins
	SPI.begin(SCK, MISO, MOSI, SS);
	//setup LoRa transceiver module
	LoRa.setPins(SS, RST, DIO0);

	if (!LoRa.begin(BAND)) {             // initialize ratio at default frequnecy
		Serial.println("LoRa init failed. Check your connections.");
		while (true);                       // if failed, do nothing
	}

	Serial.println("LoRa init succeeded.");
}

void LoRaHandler::send(String outgoing) {

}

void LoRaHandler::onReceive(int packageSize) {
	if (packageSize == 0) return;          	// if there's no packet, return

	  // read packet header bytes:
	  int recipient = LoRa.read();          // recipient address
	  byte sender = LoRa.read();            // sender address
	  byte incomingMsgId = LoRa.read();     // incoming msg ID
	  byte incomingLength = LoRa.read();    // incoming msg length

	  String incoming = "";

	  while (LoRa.available()) {
	    incoming += (char)LoRa.read();
	  }

	  if (incomingLength != incoming.length()) {   // check length for error
	    Serial.println("error: message length does not match length");
	    return;                             // skip rest of function
	  }

	  // if the recipient isn't this device or broadcast,
	  if (recipient != localAddress && recipient != 0xFF) {
	    Serial.println("This message is not for me.");
	    return;                             // skip rest of function
	  }

	  // if message is for this device, or broadcast, print details:
	  Serial.println("Received from: 0x" + String(sender, HEX));
	  Serial.println("Sent to: 0x" + String(recipient, HEX));
	  Serial.println("Message ID: " + String(incomingMsgId));
	  Serial.println("Message length: " + String(incomingLength));
	  Serial.println("Message: " + incoming);
	  Serial.println("RSSI: " + String(LoRa.packetRssi()));
	  Serial.println("Snr: " + String(LoRa.packetSnr()));
	  Serial.println();
}

void LoRaHandler::setTxPower(int powerdB) {
	if( powerdB <= 20 && powerdB >= 2 ) {
		LoRa.setTxPower(powerdB);
	} else {
		LoRa.setTxPower(17);
	}
}

void LoRaHandler::setFrequencyBand(FrequencyBand band) {
	long frequency = 0;

	switch (band) {
		case ASIA:
			frequency = 433E6;
			break;
		case EUROPE:
			frequency = 866E6;
			break;
		case NORTHAMERICA:
			frequency = 915E6;
			break;
		default:
			frequency = 433E6;
			break;
	}

	LoRa.setFrequency(frequency);
}
