/*
  Russel Reciever
  by Alexander Radevski
  Scott Mitchell
  RMIT University

  Started 10/06/16

  2.1
  - reworked. Scott
  2.0
  - code shared with Scott.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 2 as published by the Free Software Foundation.

  -------------------------------

  This program is part of the Animal Enrichment program developed for Zoos Victoria by
  RMIT University.

  The system ....
  add some info here about the system


*/

#include "SimbleeCOM.h"

// the controller
// this number can be big (<4,294,967,295) so you need to use an unsigned long
unsigned long controlESN = 0xF8F31897;
// the master
unsigned long masterESN = 0x281a72e9;

// define an object type to hold the data being sent
struct payloadStructure {
  int16_t val; //
} __attribute__((packed)); // hint to complier to pack into memory

// variable to hold our payload, paloadStructure is the variable type
payloadStructure received;
volatile bool rx_flag = false;

// for debugging purposes
boolean DEBUG = true;

// declare pin variables
const int motorPin = 5;
const int ledPinB = 9;
const int ledPinG = 11;
const int ledPinR = 12;
const int ledPinBlue = 15;
const int VoltPin = 6;
// set low and medium battery threshold
const int thresholdLow = 170;
const int thresholdMed = 220;

unsigned long motorStart = 0;
unsigned long timeout = 5000;
int val = 0;


void setup() {
  // setup serial communication
  if (DEBUG) Serial.begin(9600);

  // setup LED pins
  pinMode(ledPinB, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);
  // turn LEDs off
  digitalWrite(ledPinR, LOW);
  digitalWrite(ledPinG, LOW);
  digitalWrite(ledPinB, LOW);
  digitalWrite(ledPinBlue, LOW);
  // setup motor pin
  pinMode(motorPin, OUTPUT);
  // turn motor off
  digitalWrite(motorPin, LOW);

  // setup Simblee
  SimbleeCOM.mode = LONG_RANGE;
  SimbleeCOM.begin();
}


void loop() {
  int motorValue = 0;

  // check the battery voltage
  int voltValue = analogRead(VoltPin);
  int mapValue = map(voltValue, 0, 1023, 0, 330);
  // adjust battery indicator
  if (mapValue < thresholdLow) {
    // turn LED red
    digitalWrite(ledPinR, HIGH);
    digitalWrite(ledPinG, LOW);
  } else if (mapValue < thresholdMed) {
    // turn LED yellow
    digitalWrite(ledPinR, HIGH);
    digitalWrite(ledPinG, HIGH);
  } else {
    // turn LED green
    digitalWrite(ledPinR, LOW);
    digitalWrite(ledPinG, HIGH);
  }

  // for debuggning
  if (DEBUG) Serial.print(voltValue);
  if (DEBUG) Serial.print("    ");
  if (DEBUG) Serial.print(mapValue);

  if (rx_flag == true) {
    // get payload data
    motorValue = received.val;
    motorStart = millis();

    // for debuggning
    if (DEBUG) Serial.print("   motorValue ");
    if (DEBUG) Serial.print(received.val);

    // reset payload flag
    rx_flag = false;
  }

  // check for motor timeout
  if (millis() - motorStart > timeout) motorValue = 0;
  analogWrite(motorPin, motorValue);

  if (DEBUG) Serial.println("");

  // Ultra Low Power Delay in milliseconds - this may not work
  // also try Simblee_ULPDelay(INFINITE)
  // Simblee_ULPDelay(100);
  delay(100);
}

void SimbleeCOM_onReceive(unsigned int esn, const char *payload, int len, int rssi) {
  // check message source
  if (esn == controlESN) {
    // if the message came from the paired controller
    // set indicator LED
    digitalWrite(ledPinBlue, HIGH);

    // copy data into payload
    memcpy(&received, payload, len);

    // set recive flag
    rx_flag = true;

    delay(100);
    digitalWrite(ledPinBlue, LOW);

  } else if (esn == masterESN) {
    // if the message came from the master
    // set the module to ?
  }

  if (DEBUG) Serial.print(" rssi: ");
  if (DEBUG) Serial.print(rssi);
  if (DEBUG) Serial.print(" esn: ");
  if (DEBUG) Serial.print(esn, HEX);
  if (DEBUG) Serial.print(" payload: ");
  if (DEBUG) Serial.print(payload);
}







