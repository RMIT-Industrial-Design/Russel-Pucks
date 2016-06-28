/*
  Russel Reciever
  by Alexander Radevski
  and Scott Mitchell
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

// this puck
unsigned int thisPuck = 1;
// the controller
unsigned long controlESN = 0xF8F31897;
// the master
unsigned long masterESN = 0x281a72e9;
// all pucks
unsigned int allPucks = 999;

// define an object type to hold the data being sent
struct payloadStructure {
  int16_t val; //
} __attribute__((packed)); // hint to complier to pack into memory

// variable to hold our payload, paloadStructure is the variable type
payloadStructure received;

// for debugging purposes
boolean DEBUG = true;

// declare pin constants
const int motorPin = 5;
const int ledPinB = 9;
const int ledPinG = 11;
const int ledPinR = 12;
const int ledPinBlue = 15;
const int VoltPin = 6;
unsigned long blueLEDStart = 0;
unsigned long blueLEDtimeout = 500;

// set low and medium battery threshold
const int thresholdLow = 170;
const int thresholdMed = 220;
// set LED flash interval (to save power)
unsigned long battCheckStart = 0;
unsigned long battCheckInterval = 2000;
unsigned long battLEDtimeout = 500;

int motorValue = 0;
unsigned long motorStart = 0;
unsigned long motorTimeout = 5000;
// set by master controller
boolean puckActive = false;


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
  // check the battery voltage
  if (millis() - battCheckStart > battCheckInterval) {
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
    battCheckStart = millis();

    if (DEBUG) Serial.print(voltValue);
    if (DEBUG) Serial.print("    ");
    if (DEBUG) Serial.print(mapValue);
  }

  // check battery LED
  if (millis() - battCheckStart > battLEDtimeout) {
    // turn LED off
    digitalWrite(ledPinR, LOW);
    digitalWrite(ledPinG, LOW);
  }

  // check for active puck
  if (!puckActive) motorValue = 0;
  // check for motor timeout
  if (millis() - motorStart > motorTimeout) motorValue = 0;
  // set motor speed
  analogWrite(motorPin, motorValue);

  // turn off LED indicator
  if (millis() - blueLEDStart > blueLEDtimeout) digitalWrite(ledPinBlue, LOW);

  if (DEBUG) Serial.println("");

  // Ultra Low Power Delay in milliseconds - this does not work
  // Simblee_ULPDelay(INFINITE) - no bluetooth interupt
  // Simblee_ULPDelay(100);
  delay(100);
}

void SimbleeCOM_onReceive(unsigned int esn, const char *payload, int len, int rssi) {
  // copy data into payload
  memcpy(&received, payload, len);

  // check message source
  if (esn == masterESN) {
    // if the paylod is this Simblee's ESN then make this puck active
    if (received.val == thisPuck || received.val == allPucks) puckActive = true;
    else puckActive = false;
  } else if (puckActive) {
    // if the message came from the paired controller
    if (esn == controlESN) {
      // set indicator LED
      digitalWrite(ledPinBlue, HIGH);
      blueLEDStart = millis();
      // set motor value
      motorValue = received.val;
      motorStart = millis();
    }
  }

  if (DEBUG) Serial.print(" rssi: ");
  if (DEBUG) Serial.print(rssi);
  if (DEBUG) Serial.print(" esn: ");
  if (DEBUG) Serial.print(esn, HEX);
  if (DEBUG) Serial.print(" payload: ");
  if (DEBUG) Serial.print(payload);
}







