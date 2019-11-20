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

	setLocalAddress(0xAA);
	setDestination(0xFF);
	setFrequency(ASIA);
}

LoRaHandler::~LoRaHandler() {
	// TODO Auto-generated destructor stub
}

LoRaHandler::LoRaHandler(FrequencyBand band, byte localAddress, byte destination) {
	if(!Serial) {
		Serial.begin(115200);
	}

	setFrequency(band);
	setLocalAddress(localAddress);

	if ( localAddress != destination ) {
		setDestination(destination);
	} else {
		setDestination(0xFF);

		Serial.println("[lora] error: localAddress and destination match!");
		Serial.println("[lora] => changed destination address to broadcast: 0xFF.");
	}
}

void LoRaHandler::initialize(byte localAddress, byte destination) {
	// set local dev	LoRaHandler(FrequencyBand band, byte localAddress, byte destination);	LoRaHandler(FrequencyBand band, byte localAddress, byte destination);	LoRaHandler(FrequencyBand band, byte localAddress, byte destination);ice and destination address
	setLocalAddress(localAddress);
	setDestination(destination);

	//SPI LoRa pins
	SPI.begin(SCK, MISO, MOSI, SS);

	//setup LoRa transceiver module
	LoRa.setPins(SS, RST, DIO0);

	if (!LoRa.begin(getFrequency())) {      // initialize ratio at default frequnecy
		Serial.println("[lora] error: LoRa init failed. Check your connections.");
		while (true);                       // if failed, do nothing
	}

	Serial.println("[lora] LoRa init succeeded.");
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
	setFrequency(band);
	LoRa.setFrequency(getFrequency());
}


byte LoRaHandler::getDestination() {
	return destination;
}

void LoRaHandler::setDestination(byte destination) {
	this->destination = destination;
}

byte LoRaHandler::getLocalAddress() {
	return localAddress;
}

void LoRaHandler::setLocalAddress(byte localAddress) {
	this->localAddress = localAddress;
}

long LoRaHandler::getFrequency() {
	return frequency;
}

void LoRaHandler::setFrequency(FrequencyBand band) {
	switch (band) {
		case ASIA:
			this->frequency = 433E6;
			break;
		case EUROPE:
			this->frequency = 866E6;
			break;
		case NORTHAMERICA:
			this->frequency = 915E6;
			break;
		default:
			this->frequency = 433E6;
			break;
	}
}
