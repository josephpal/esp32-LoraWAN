/*
 * LoRaHandler.h
 *
 *  Created on: Nov 13, 2019
 *      Author: joseph
 *
 *  version:		0.1
 *
 *  description:	For setting up different default device frequencies, change #define BAND regarding your
 *  				region
 *  				-> 433E6 for Asia
 * 					-> 866E6 for Europe
 * 					-> 915E6 for North America
 *
 *					Two device addresses are necessary to be declared:
 *					localAddress -> if you want to have a peer to peer communication;
 *									has to be the same on Sender and Receiver, otherwise the incoming message will be ignored
 *					destination  ->	by default, it is set to the broadcast address, is used to receive global message from different clients.
 *
 *					Example:		client 1 (destination: 0xFF, localAddress: 0xBB) => 1->2,3 (broadcast)
 *									client 2 (destination: 0xBB, localAddress: 0xCC) => 2->1 (direct), 2->3 doesn't work
 *									client 3 (destination: 0xEE, localAddress: 0xDD) => can not call anyone
 *  notifications:
 *
 *
 *  src:	[1] https://github.com/sandeepmistry/arduino-LoRa/blob/master/examples/LoRaDuplex/LoRaDuplex.ino
 *  		[2] https://github.com/sandeepmistry/arduino-LoRa
 *  		[3] LoRa API -> https://github.com/sandeepmistry/arduino-LoRa/blob/master/API.md
 */

#ifndef LORAHANDLER_H_
#define LORAHANDLER_H_

#include "Arduino.h"
#include "Cipher.h"
#include "Mutex.h"
#include "OLEDDisplay.h"

#include <SPI.h>
#include <LoRa.h>

/*
 *	default definition of the pins used by the LoRa transceiver module
 *	model (TTGO):
*/
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define SS_LORA 18
#define RST_LORA 14
#define DIO0_LORA 26

/*
 * 	default device frequency (433MHz)
 * 	can be changed during the call of the constructor or initialize(...)
*/
enum FrequencyBand {
	ASIA,
	EUROPE,
	NORTHAMERICA
};

/*
 * 	enumeration to represent the received package status after reading and saving
 * 	the header information
*/
enum PackageHeader {
	PKG_EMPTY,
	PKG_MSGLENGTH_MISSMATCH,
	PKG_WRONG_RECIPIENT,
	PKG_LOSS,
	PKG_CONFIRMATION,
	PKG_RESENT,
	PKG_NEWMESSAGE,
	PKG_UNKOWN
};

/*
 * 	enumeration to represent the sent package status after receiving
 * 	a timeout, a confirmation or a resent request
*/
enum MsgStatus {
	MSG_CONFIRMED,
	MSG_TIMED_OUT,
	MSG_RESENT_REQUEST,
	MSG_UNKNOWN
};

class LoRaHandler {
public:

	/* ================================= device setup ================================== */

	/** default constructor, he will set all default settings by calling the function
	 *  setupDefaultSettings() and the constructor will clear the class properties.
	 * 	For the LoRa device, he will use pin numbers for the TTGO ESP32 LoRa device. If you want to change
	 * 	them , use the overloaded constructor or the functions setSPIPins() or setLORAPins(). After that
	 * 	call initialize() to start the LoRa device with the preset settings.
	 *
	 *  @param	---
	 *  @return	---
	*/
	LoRaHandler();
	~LoRaHandler();

	/** overloaded constructor to set the frequency band by the help of a enumeration. It can be also used to set the
	 * 	default device address as well as the destination address for the communication partner. All other settings will be
	 * 	used by calling the function setupDefaultSettings().
	 *
	 *  @param	frequency on which the LoRa module should run (ASIA, EUROPE, NORTHAMERICA)
	 *  @param	local device address in byte (e.g. 0xAA)
	 *  @param	destination device address in byte (e.g. 0xBB). If local == destination, destination will be set to 0xFF (broadcast)
	 *  @return	---
	*/
	LoRaHandler(FrequencyBand band, byte localAddress, byte destination);

	/** Set the default device address as well as the destination address for the communication partner. This function will also
	 * 	start the SPI communication with the LoRa device and will initialize it.
	 *
	 * 	@param	local device address in byte (e.g. 0xAA)
	 *  @param	destination device address in byte (e.g. 0xBB). If local == destination, destination will be set to 0xFF (broadcast)
	 *  @return	---
	*/
	void initialize(byte localAddress, byte destination);

	/** If we want to encrypt our transmission (only the message itself, not the whole payload header), we can define a encryption
	 * 	key to achieve that aim. The receiver must know this key as well to decrypt the received message.
	 *
	 *  @param
	 *  @return
	*/
	void initEncryption(char * key);

	/** ...
	 *
	 *  @param
	 *  @return
	*/
	void setSPIPins(int pinSCK, int pinMISO, int pinMOSI, int pinSS);

	/** ...
	 *
	 *  @param
	 *  @return
	*/
	void setLORAPins(int pinSS_LORA, int pinRST_LORA, int pinDIO0_LORA);

	/**	...
	 *
	 *	@param	...
	 *	@return	---
	*/
	void setupDefaultSettings();

	/* ================================ device functions ================================ */

	/** ...
	 *
	 *  @param
	 *  @return
	*/
	void send(String outgoing);

	/** ...
	 *
	 *  @param
	 *  @return
	*/
	void send(String outgoing, byte destination);

	/** ...
	 *
	 *  @param
	 *  @return
	*/
	void sendConfirmation(byte destination);

	/** ...
	 *
	 *  @param
	 *  @return
	*/
	void resentRequest(byte destination);

	/** ...
	 *
	 *  @param
	 *  @return
	*/
	void sendPackage(String outgoing, byte destination);

	/** ...
	 *
	 *  @param
	 *  @return
	*/
	bool onReceive(int packageSize);

	/** ...
	 *
	 *  @param	...
	 *  @return	---
	*/
	PackageHeader readPackageHeader(int packageSize);

	/** ...
	 *
	 *  @param	...
	 *  @return	---
	*/
	void clearPackageHeader();

	/**	...
	 *
	 * 	@param	...
	 * 	@return	---
	 */
	void displayPackageHeader();

	/** ...
	 *
	 *  @param
	 *  @return
	*/
	void startReceiveListner();

	/** ...
	 *
	 *  @param
	 *  @return
	*/
	bool checkPackageLoss(byte currentPacketID);

	/** ...
	 *
	 *  @param	...
	 *  @return	---
	*/
	void updateMsgStatus(MsgStatus mStatus, bool msgConfirmed);

	/** ...
	 *
	 *  @param
	 *  @return
	*/
	void setTxPower(int powerdBm);

	/** set the device frequency property and at the same time the real frequency of the device itself
	 *
	 *  @param	433E6, 868E6, 915E6
	 *  @return	---
	*/
	void setFrequencyBand(FrequencyBand band);

	/* =============================== Getters and Setters =============================== */

	byte getDestination();

	void setDestination(byte destination);


	byte getLocalAddress();

	void setLocalAddress(byte localAddress);


	long getFrequency();

	/** this will only set the device frequency property of the class and not the device frequency
	 *
	 *  @param	frequency band enum (ASIA -> 433E6, EUROPE -> 866E6, NORTHAMERICA->915E6)
	 *  @return ---
	*/
	void setFrequency(FrequencyBand band);


	byte getLastPacketId();

	void setLastPacketId(byte lastPacketId);


	bool isActivateEncryption();

	void setActivateEncryption(bool activateEncryption);


	String getReceivedMessage();

	void setReceivedMessage(String receivedMessage);


	bool isMsgConfirmed();

	void setMsgConfirmed(bool msgConfirmed);

	bool isMsgResent();

	void setMsgResent(bool msgResent);


	MsgStatus& getMsgStatus();

	void setMsgStatus(MsgStatus mStatus);


	void setSenderAddress(byte address);

	byte getSenderAddress();

private:
	/*
	 * 	receivedMessage: message from type String, which is received
	 * 	msgStatus: contains the current status of a transmitted package
	 * 		-> receivedConfirmation if we received from our communication partner a confirmation
	 * 		-> timedOut if we wont receive a confirmation in less than ~2 seconds
	 *
	 * 	If we receive a new message, the received header with all its information will be saved in
	 * 	the package[] template. The string array package[] will contain the following content:
	 *
	 * 		--------------------- MSG HEADER ---------------------
	 *
	 * 			package[0]	->	destination address
	 * 			package[1]	->	sender address
	 * 			package[2]	->	message ID
	 * 			package[3]	->	payload length
	 * 			package[4]	->	payload
	 * 			package[5]	->	RSSI
	 * 			package[6]	->	SNR
	 *
	 */
	String receivedMessage, lastMessage;

	//String msgStatus;
	MsgStatus msgStatus;

	String package[7];

	/*
	 * 	default mutex to handle bidirectional mode of the LoRa device, if you want to send,
	 * 	the receiver thread has to wait, and also the opposite way
	 */
	Mutex receiveMutex, sendMutex, msgMutex;
	bool msgConfirmed, msgResent;

	/*
	 * 	msgCount:		count of outgoing messages; amount of message, which are already transmitted
	 * 	localAddress:	address of this device in hex, e.g 0xAA
	 * 	destination:	address of the communication partner, like 0xBB, default: 0xFF (broadcast address)
	 */
	byte msgCount;				// count of outgoing messages
	byte localAddress;			// address of this devMsgStatusice
	byte destination;			// destination to send to
	byte senderAddress;			// last saved sender address from which we received the last message

	/*
	 *	LoRa WAN device frequency (class property!) (433, 866, 915 MHz)
	 */
	long frequency;

	/*
	 *	package ID will be saved after we receive a message,
	 */
	byte lastPacketID;

	/*
	 * 	device configuration
	 * 	-> SCK, MISO, MOSI, SS (SPI)
	 * 	-> SS_LORA, RST_LORA, DIO0_LORA
	 */
	int pinSCK, pinMISO, pinMOSI, pinSS, pinSS_LORA, pinRST_LORA, pinDIO0_LORA;

	/*
	 *  encryption class
	 */
	Cipher * cipher;

	bool activateEncryption;

	/*
	 * 	QueueHandle_t object to communicate between the threads (receive package confirmation)
	 */
	QueueHandle_t *queue;
	int queueSize;
};

#endif /* LORAHANDLER_H_ */
