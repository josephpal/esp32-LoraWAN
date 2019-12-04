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
	OLEDDisplay * oled = new OLEDDisplay();

	while(runTask) {
		if ( lora == NULL ) {
			Serial.println("[lora-task] error: LoRaHandler not defiened as global pointer.");
			Serial.println("[lora-task] Can't receive messages. Aborted!");

			runTask = false;
			break;
		} else {
			// parse for a packet, and call onReceive with the result, if true, a package is received:
			if( lora->onReceive(LoRa.parsePacket()) ) {
				Serial.println("[lora-task] received message: => " + lora->getReceivedMessage() + "\n");

				oled->OLEDfloattext(lora->getReceivedMessage(), 1);
			}
		}

		delay(10);
	}

	vTaskDelete(NULL);
}

void confirmationListener(void *param) {
	bool runTask = true;
	int timeOutCounter = 0;

	Serial.println("[lora-task] waiting for package confirmation ...");

	while(runTask) {
		if ( lora == NULL ) {
			Serial.println("[lora-task] error: LoRaHandler not defiened as global pointer.");
			Serial.println("[lora-task] Can't receive messages. Aborted!");

			runTask = false;
			break;
		} else {

			timeOutCounter++;

			if( lora->isMsgConfirmed() == true ) {
				runTask = false;
				lora->updateMsgStatus("receivedConfirmation", false);
			}

			if( timeOutCounter >= 200 ) {
				runTask = false;
				lora->updateMsgStatus("timedOut", false);
			}
		}

		delay(10);
	}

	vTaskDelete(NULL);
}

/* ================================= device setup ================================== */

LoRaHandler::LoRaHandler() {
	setupDefaultSettings();
}

LoRaHandler::~LoRaHandler() {
	delete cipher;
	delete queue;
}

LoRaHandler::LoRaHandler(FrequencyBand band, byte localAddress, byte destination) {
	setupDefaultSettings();

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
	// set local device and destination address
	setLocalAddress(localAddress);
	setDestination(destination);

	// SPI LoRa pins
	SPI.begin(this->pinSCK, this->pinMISO, this->pinMOSI, this->pinSS);

	// setup LoRa transceiver module
	LoRa.setPins(this->pinSS_LORA, this->pinRST_LORA, this->pinDIO0_LORA);

	if (!LoRa.begin(getFrequency())) {      // initialize ratio at default frequnecy
		Serial.println("[lora] error: LoRa init failed. Check your connections.");
		while (true);                       // if failed, do nothing
	}

	Serial.println("[lora] LoRa init succeeded.\n");
}

void LoRaHandler::initEncryption(char * key) {
	setActivateEncryption(true);

	Serial.println("[lora] init message encryption ...");

	cipher = new Cipher(key);

	Serial.println("[lora] done.\n");
}

void LoRaHandler::setSPIPins(int pinSCK, int pinMISO, int pinMOSI, int pinSS) {
	this->pinSCK = pinSCK;
	this->pinMISO = pinMISO;
	this->pinMOSI = pinMOSI;
	this->pinSS = pinSS;
}

void LoRaHandler::setLORAPins(int pinSS_LORA, int pinRST_LORA,
		int pinDIO0_LORA) {
	this->pinSS_LORA = pinSS_LORA;
	this->pinRST_LORA = pinRST_LORA;
	this->pinDIO0_LORA = pinDIO0_LORA;
}

void LoRaHandler::setupDefaultSettings() {
	if(!Serial) {
		Serial.begin(115200);
	}

	// default device settings
	pinSCK = SCK;
	pinMISO = MISO;
	pinMOSI = MOSI;
	pinSS = SS;
	pinSS_LORA = SS_LORA;
	pinRST_LORA = RST_LORA;
	pinDIO0_LORA = DIO0_LORA;

	Serial.println("done.");

	/* default values for the properties of the class */
	receivedMessage = "";
	msgStatus = "";
	msgCount = 0;

	clearPackageHeader();

	sendMutex.unlock();
	receiveMutex.unlock();
	msgConfirmed = false;

	/* lora device settings */
	localAddress = 0xAA;
	destination = 0xFF;
	frequency = 433E6;
	senderAddress = 0x00;

	/* init ID: packet IDs will go from 0...255 */
	lastPacketID = 0;

	/* encryption class */
	cipher = new Cipher();
	activateEncryption = false;

	/* Queue handle object to communicate between send and receive task */
	queue = new QueueHandle_t();
	this->queueSize = 10;

	*queue = xQueueCreate( queueSize, sizeof( String ) );
}

/* ================================ device functions ================================ */

void LoRaHandler::send(String outgoing) {
	send(outgoing, getDestination());
}

void LoRaHandler::send(String outgoing, byte destination) {
	/* send message */
	sendPackage(outgoing, destination);

	/* message sent, now create a thread in the background to listen if we received the confirmation successfully */
	xTaskCreatePinnedToCore(
						confirmationListener,   /* Function to implement the task */
						"confirmationListener", /* Name of the task */
						2048,	      			/* Stack size in words */
						NULL,       			/* Task input parameter */
						0,          			/* Priority of the task */
						NULL,       			/* Task handle. */
						0);  					/* Core where the task should run */

	/* wait until we received a confirmation or a timeout */
	xQueueReceive(*queue, &msgStatus, portMAX_DELAY);

	if( msgStatus == "receivedConfirmation" ) {
		Serial.println("[lora] msg confirmation received. Allowed to send a new message now.\n");
	} else if ( msgStatus == "timedOut" ) {
		Serial.println("[lora] error: msg confirmation timeout.\n");
	}

	delay(1000);
}

void LoRaHandler::sendConfirmation(byte destination) {
	/* send confirmation message */
	sendPackage("received", destination);
}

void LoRaHandler::sendPackage(String outgoing, byte destination) {
	sendMutex.lock();

	if( isActivateEncryption() ) {
		outgoing = cipher->encryptString(outgoing);
		Serial.println("[lora] sending encrypted message: " + outgoing + " to 0x" + String(destination, HEX));
	} else {
		Serial.println("[lora] sending message: " + outgoing + " to 0x" + String(destination, HEX));
	}

	LoRa.beginPacket();                   // start packet
	LoRa.write(destination);         	  // add destination address
	LoRa.write(getLocalAddress());        // add sender address
	LoRa.write(msgCount);                 // add message ID
	LoRa.write(outgoing.length());        // add payload length
	LoRa.print(outgoing);                 // add payload
	LoRa.endPacket();                     // finish packet and send it
	msgCount++;                           // increment message ID

	sendMutex.unlock();
}

bool LoRaHandler::onReceive(int packageSize) {
	receiveMutex.lock();

	bool returnCondition = false;

	switch ( readPackageHeader(packageSize) ) {
		case PKG_EMPTY:
			returnCondition = false;
			break;

		case PKG_MSGLENGTH_MISSMATCH:
			Serial.println("[lora] error: message length does not match length in header.");
			returnCondition = false;
			break;

		case PKG_WRONG_RECIPIENT:
			Serial.println("[lora] warning: This message is not dedicated to me.");
			break;

		case PKG_LOSS:
			Serial.println("[lora] error: package loss detected!\n");
			break;

		case PKG_CONFIRMATION:
			msgConfirmed = true;

			returnCondition = true;
			break;

		case PKG_NEWMESSAGE:
			/* display received package header containing our message */
			displayPackageHeader();

			/* confirm receiving the package */
			sendConfirmation(getSenderAddress());

			returnCondition = true;
			break;

		default:
			break;
	}

	receiveMutex.unlock();

	return returnCondition;
}

PackageHeader LoRaHandler::readPackageHeader(int packageSize) {
	if (packageSize == 0) {
		/* if there's no packet -> package empty */
		return PKG_EMPTY;
	} else {
		/* read packet header bytes */

		int recipient = LoRa.read();          // recipient address
		byte sender = LoRa.read();            // sender address
		byte incomingMsgId = LoRa.read();     // incoming msg ID
		byte incomingLength = LoRa.read();    // incoming msg length

		String incoming = "";

		while (LoRa.available()) {
			incoming += (char)LoRa.read();
		}

		if (incomingLength != incoming.length()) {
			/* check length for error */
			return PKG_MSGLENGTH_MISSMATCH;
		}

		if (recipient != localAddress && recipient != 0xFF) {
			/* if the recipient isn't this device or broadcast address */
			return PKG_WRONG_RECIPIENT;
		}

		if ( checkPackageLoss(incomingMsgId) == true ) {
			/* check of package loss */
			setLastPacketId(incomingMsgId);

			return PKG_LOSS;
		} else {
			setLastPacketId(incomingMsgId);
		}

		/* is it necessary to decrypt the received message in the package? */
		if( isActivateEncryption() ) {
			setReceivedMessage(cipher->decryptString(incoming));
		} else {
			setReceivedMessage(incoming);
		}

		/* write received information in the header template */
		for (int i = 0; i < (sizeof(package) / sizeof(package[0])); ++i) {
			switch (i) {
				case 0:
					setSenderAddress(sender);
					package[0] = String(getSenderAddress(), HEX);
					break;

				case 1:
					package[1] = String(recipient, HEX);
					break;

				case 2:
					package[2] = String(incomingMsgId);
					break;

				case 3:
					package[3] = String(incomingLength);
					break;

				case 4:
					package[4] = getReceivedMessage();
					break;

				case 5:
					package[5] = String(LoRa.packetRssi());
					break;

				case 6:
					package[6] = String(LoRa.packetSnr());
					break;

				default:
					clearPackageHeader();
					break;
			}
		}
	}

	return PKG_NEWMESSAGE;
}

void LoRaHandler::clearPackageHeader() {
	for (int i = 0; i < (sizeof(package) / sizeof(package[0])); ++i) {
		package[i] = "";
	}
}

void LoRaHandler::displayPackageHeader() {
	Serial.println("[lora] Received from: 0x" + package[0]);
	Serial.println("[lora] Sent to: 0x" + package[1]);
	Serial.println("[lora] Message ID: " + package[2]);
	Serial.println("[lora] Message length: " + package[3]);
	Serial.println("[lora] Message: " + package[4]);
	Serial.println("[lora] RSSI: " + package[5]);
	Serial.println("[lora] SNR: " + package[6]);
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

	if(lID == 255) {
		lID = -1;
	}

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

void LoRaHandler::updateMsgStatus(String msgStatus, bool msgConfirmed) {
	this->msgStatus = msgStatus;
	this->msgConfirmed = msgConfirmed;

	/* package confirmation received or timeout */
	xQueueSend(*queue, &msgStatus, portMAX_DELAY);
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

/* ======================= class properties getters & setters  =======================*/

byte LoRaHandler::getDestination() {
	return destination;
}

void LoRaHandler::setDestination(byte destination) {
	Serial.println("[lora] Setting destination device address to 0x" + String(destination, HEX) + ".");
	this->destination = destination;
}

byte LoRaHandler::getLocalAddress() {
	return localAddress;
}

void LoRaHandler::setLocalAddress(byte localAddress) {
	Serial.println("[lora] Setting local device address to 0x" + String(localAddress, HEX) + ".");
	this->localAddress = localAddress;
}

long LoRaHandler::getFrequency() {
	return frequency;
}

void LoRaHandler::setFrequency(FrequencyBand band) {
	Serial.print("[lora] Setting LoRaWAN frequency to ");
	switch (band) {
		case ASIA:
			this->frequency = 433E6;
			Serial.println("433E6");
			break;
		case EUROPE:
			this->frequency = 866E6;
			Serial.println("866E6");
			break;
		case NORTHAMERICA:
			this->frequency = 915E6;
			Serial.println("915E6");
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

bool LoRaHandler::isActivateEncryption() {
	return activateEncryption;
}

void LoRaHandler::setActivateEncryption(bool activateEncryption) {
	Serial.println("[lora] Setting message encryption status to " + String(activateEncryption ? "true." : "false."));
	this->activateEncryption = activateEncryption;
}

String LoRaHandler::getReceivedMessage() {
	return receivedMessage;
}

void LoRaHandler::setReceivedMessage(String receivedMessage) {
	this->receivedMessage = receivedMessage;
}

bool LoRaHandler::isMsgConfirmed() {
	return msgConfirmed;
}

void LoRaHandler::setMsgConfirmed(bool msgConfirmed) {
	this->msgConfirmed = msgConfirmed;
}

String & LoRaHandler::getMsgStatus() {
	return msgStatus;
}

void LoRaHandler::setMsgStatus(String msgStatus) {
	this->msgStatus = msgStatus;
}

void LoRaHandler::setSenderAddress(byte address) {
	senderAddress = address;
}

byte LoRaHandler::getSenderAddress() {
	return senderAddress;
}
