/*
  Russel Master
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

// define an object type to hold the data being sent
struct payloadStructure {
  int16_t val;
} __attribute__((packed)); // hint to complier to pack into memory

// variable to hold our payload, paloadStructure is the variable type
payloadStructure payload;

// for debugging purposes
boolean DEBUG = true;

// declare pin constants
// switch is connected to an analog pin via a resitor array
const int switchPin = 3;
const int ledPinB = 9;
const int ledPinG = 11;
const int ledPinR = 12;
const int VoltPin = 6;

// set low and medium battery threshold
const int thresholdLow = 170;
const int thresholdMed = 220;
// set LED flash interval (to save power)
unsigned long battCheckStart = 0;
unsigned long battCheckInterval = 6000;
unsigned long battLEDtimeout = 1000;


void setup() {
  // setup serial communication
  if (DEBUG) Serial.begin(9600);

  // setup LED pins
  pinMode(ledPinB, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinR, OUTPUT);
  // turn LEDs off
  digitalWrite(ledPinR, LOW);
  digitalWrite(ledPinG, LOW);
  digitalWrite(ledPinB, LOW);

  // setup Simblee
  SimbleeCOM.mode = LONG_RANGE;
  SimbleeCOM.begin();
}

void loop() {
  // system state
  boolean systemOn = true;
  // read switch value
  int switchValue = analogRead(switchPin);
  if (DEBUG) Serial.print("switch read: ");
  if (DEBUG) Serial.print(switchValue);
  
  // asign payload value based on switch position
  // switch is connected to an array of 12 equal resistors
  if (switchValue < 53) payload.val = 999; // master off
  else if (switchValue < 146) payload.val = 9; // all controllers off
  else if (switchValue < 239) payload.val = 8;
  else if (switchValue < 332) payload.val = 7;
  else if (switchValue < 425) payload.val = 6; //yellow puck on
  else if (switchValue < 518) payload.val = 5; //orange puck on
  else if (switchValue < 611) payload.val = 2; //dark blue puck on
  else if (switchValue < 704) payload.val = 3; //red puck on
  else if (switchValue < 797) payload.val = 4; //light blue puck on
  else if (switchValue < 890) payload.val = 1; // green puck on
  else if (switchValue < 983) payload.val = 0;
  else payload.val = 0; // all controllers on

  // send the payload
  SimbleeCOM.send((char*)&payload, sizeof(payload));
  digitalWrite(ledPinR, LOW);
  digitalWrite(ledPinG, LOW);
  digitalWrite(ledPinB, HIGH);
  delay(100);
  digitalWrite(ledPinB, LOW);

  if (DEBUG) Serial.print("  payload: ");
  if (DEBUG) Serial.print(payload.val);

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

    if (DEBUG) Serial.print(" analog read: ");
    if (DEBUG) Serial.print(voltValue);
    if (DEBUG) Serial.print(" volts x 10: ");
    if (DEBUG) Serial.print(mapValue);
  }

  // check battery LED
  if (millis() - battCheckStart > battLEDtimeout) {
    // turn LED off
    digitalWrite(ledPinR, LOW);
    digitalWrite(ledPinG, LOW);
  }

  if (DEBUG) Serial.println("");

  // Ultra Low Power Delay in milliseconds - does this work?
  if (payload.val == 0) Simblee_ULPDelay(3000);
  else Simblee_ULPDelay(200);
}

