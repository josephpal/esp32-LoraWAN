/*
 * LoRaHandler.cpp
 *
 *  Created on: Nov 13, 2019
 *      Author: joseph
 */

#include "LoRaHandler.h"

extern LoRaHandler *lora;

void receiveListner(void *param) {

	bool runTask = true;

	while(runTask) {
		if ( lora == NULL ) {
			Serial.println("[lora-task] error: LoRaHandler not defiened as global pointer.");
			Serial.println("[lora-task] Can't receive messages. Aborted!");

			runTask = false;
			break;
		} else {
			// parse for a packet, and call onReceive with the result:
			lora->onReceive(LoRa.parsePacket());
		}

		delay(10);
	}

	vTaskDelete(NULL);
}

LoRaHandler::LoRaHandler() {
	if(!Serial) {
		Serial.begin(115200);
	}

	outgoing = "";
	msgCount = 0;

	setLocalAddress(0xAA);
	setDestination(0xFF);
	setFrequency(ASIA);

	// init ID: packet IDs will go from 0...255
	setLastPacketId(0);
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

	// init ID: packet IDs will go from 0...255
	setLastPacketId(0);
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
	Serial.println("[lora] sending message: " + outgoing);

	LoRa.beginPacket();                   // start packet
	LoRa.write(getDestination());         // add destination address
	LoRa.write(getLocalAddress());        // add sender address
	LoRa.write(msgCount);                 // add message ID
	LoRa.write(outgoing.length());        // add payload length
	LoRa.print(outgoing);                 // add payload
	LoRa.endPacket();                     // finish packet and send it
	msgCount++;                           // increment message ID
}

void LoRaHandler::onReceive(int packageSize) {
	if (packageSize == 0) return;          	// if there's no packet, return

	// read packet header bytes:
	int recipient = LoRa.read();          // recipient address
	byte sender = LoRa.read();            // sender address
	byte incomingMsgId = LoRa.read();     // incoming msg ID
	byte incomingLength = LoRa.read();    // incoming msg length



	// check of package loss
	if ( checkPackageLoss(incomingMsgId) == true ) {
		Serial.println("[lora] error: package loss detected!");
		setLastPacketId(incomingMsgId);

		return;
	} else {
		setLastPacketId(incomingMsgId);
	}

	String incoming = "";

	while (LoRa.available()) {
		incoming += (char)LoRa.read();
	}

	// check length for error, if true, skip rest of function
	if (incomingLength != incoming.length()) {
		Serial.println("[lora] error: message length does not match length");
		return;
	}

	// if the recipient isn't this device or broadcast, if true, skip rest of function
	if (recipient != localAddress && recipient != 0xFF) {
		Serial.println("[lora] warning: This message is not for me.");
		return;
	}

	// if message is for this device, or broadcast, print details:
	Serial.println("[lora] Received from: 0x" + String(sender, HEX));
	Serial.println("[lora] Sent to: 0x" + String(recipient, HEX));
	Serial.println("[lora] Message ID: " + String(incomingMsgId));
	Serial.println("[lora] Message length: " + String(incomingLength));
	Serial.println("[lora] Message: " + incoming);
	Serial.println("[lora] RSSI: " + String(LoRa.packetRssi()));
	Serial.println("[lora] Snr: " + String(LoRa.packetSnr()));
	Serial.println();
}

void LoRaHandler::startReceiveListner() {
	Serial.println("[lora] Starting receiveListner for incomming messages ...");

	xTaskCreate(
		receiveListner,         /* Task function. */
		"receiveListener",  	/* String with name of task. */
		10000,            		/* Stack size in bytes. */
		NULL,     				/* Parameter passed as input of the task */
		1,                		/* Priority of the task. */
		NULL);            		/* Task handle. */

}

bool LoRaHandler::checkPackageLoss(byte currentPacketID) {
	bool returnCondition;

	int cID = (int)currentPacketID;
	int lID = (int)getLastPacketId();

	if( (lID + 1) != cID) {
		Serial.println("[lora] warning: package loss! Expected msgID: " + String(lID + 1)
							+ " but received msgID: " + String(cID) + "\n");
		returnCondition = true;
	} else if ( cID == lID ) {
		Serial.println("[lora] warning: out of sync! Received similar msgID's: " +  String(cID) + "\n");
		returnCondition = true;
	} else {
		returnCondition = false;
	}

	return returnCondition;
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

byte LoRaHandler::getLastPacketId() {
	return lastPacketID;
}

void LoRaHandler::setLastPacketId(byte lastPacketId) {
	lastPacketID = lastPacketId;
}
