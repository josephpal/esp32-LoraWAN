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
 *					Example:		client 1 (destination: 0xFF, localAddress: 0xBB)
 *									client 2 (destination: 0xBB, localAddress: 0xCC)
 *									client 2 (destination: 0xBB, localAddress: 0xDD)
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
#include <SPI.h>
#include <LoRa.h>

/*
 *	default definition of the pins used by the LoRa transceiver module
 *	model:
 */
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

/*
 * 	default device frequency (433MHz)
 * 	can be changed during the call of the constructor or initialize(...)
 */
#define BAND 433E6

enum FrequencyBand {
	ASIA,
	EUROPE,
	NORTHAMERICA
};

class LoRaHandler {
public:
	/** ...
	 *
	 *  @param
	 *  @return
	*/
	LoRaHandler();
	~LoRaHandler();

	/** ...
	 *
	 *  @param
	 *  @return
	*/
	void initialize();


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
	void onReceive(int packageSize);

	/** ...
	 *
	 *  @param
	 *  @return
	*/
	void setTxPower(int powerdB);

	/** ...
	 *
	 *  @param
	 *  @return
	*/
	void setFrequencyBand(FrequencyBand band);

private:
	/*
	 * 	outgoing: message from type String, which will be transmitted
	 */
	String outgoing;

	/*
	 * 	msgCount:		count of outgoing messages; amount of message, which are already transmitted
	 * 	localAddress:	address of this device in hex, e.g 0xBB
	 * 	destination:
	 */
	byte msgCount;				// count of outgoing messages
	byte localAddress;			// address of this device
	byte destination;			// destination to send to

	/*
	 *
	 */

};

#endif /* LORAHANDLER_H_ */
