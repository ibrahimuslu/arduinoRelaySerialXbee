/**
   Copyright (c) 2009 Andrew Rapp. All rights reserved.

   This file is part of XBee-Arduino.

   XBee-Arduino is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   XBee-Arduino is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <XBee.h>
#include <SoftwareSerial.h>

/*
  This example is for Series 2 XBee
  Receives a ZB RX packet and prints the packet to softserial
*/

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();
ZBTxStatusResponse txStatus = ZBTxStatusResponse();
// Define NewSoftSerial TX/RX pins
// Connect Arduino pin 8 to TX of usb-serial device
uint8_t ssRX = 0;
// Connect Arduino pin 9 to RX of usb-serial device
uint8_t ssTX = 1;
// Remember to connect all devices to a common Ground: XBee, Arduino and USB-Serial device
SoftwareSerial nss(ssRX, ssTX);

void setup() {
  // start serial
  Serial.begin(9600);
  pinMode(ssRX, INPUT);
  pinMode(ssTX, OUTPUT);

  xbee.setSerial(Serial);
  nss.begin(9600);
  Serial.println("Starting up!");
}

// continuously reads packets, looking for ZB Receive or Modem Status
void loop() {

  xbee.readPacket();
  uint8_t payload[] = { 'D', '0', 'C', 'D', '1','C', 'D', '2','C', 'D', '3','C' };


  if (xbee.getResponse().isAvailable()) {
    // got something

    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      // got a zb rx packet

      // now fill our zb rx class
      xbee.getResponse().getZBRxResponse(rx);

      nss.println("Got an rx packet!");

      if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
        // the sender got an ACK
        Serial.println("packet acknowledged");
      } else {
        Serial.println("packet not acknowledged");
      }

      Serial.print("checksum is ");
      Serial.println(rx.getChecksum(), HEX);

      Serial.print("packet length is ");
      Serial.println(rx.getPacketLength(), DEC);
      Serial.println((char*)rx.getData());
      if (strcmp((char*)rx.getData(), "Units")==0) {
        // SH + SL Address of receiving XBee
        XBeeAddress64 addr64 = XBeeAddress64(rx.getRemoteAddress64());
        ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
        ZBTxStatusResponse txStatus = ZBTxStatusResponse();
        xbee.send(zbTx);
      }

    }
    else if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);

      // get the delivery status, the fifth byte
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        // success.  time to celebrate
        Serial.println("\nSuccess");
      } else {
        // the remote XBee did not receive our packet. is it powered on?
        Serial.println("Muccess");
      }
    }
  } else if (xbee.getResponse().isError()) {
    Serial.print("oh no!!! error code:");
    Serial.println(xbee.getResponse().getErrorCode());
  }
}
