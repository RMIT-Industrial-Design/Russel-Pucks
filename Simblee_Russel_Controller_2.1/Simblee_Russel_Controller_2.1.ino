/*
  Russel Controller
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
const int potPin = 3;
const int motorPin = 5;
const int ledPinB = 9;
const int ledPinG = 11;
const int ledPinR = 12;
const int ledPinBlue = 15;
const int voltPin = 6;

// set low and medium battery threshold
const int thresholdLow = 170;
const int thresholdMed = 220;
// set LED flash interval (to save power)
unsigned long battCheckStart = 0;
unsigned long battCheckInterval = 5000;
unsigned long battLEDtimeout = 1000;


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
  // read pot value
  int potValue = analogRead(potPin);
  // add a buffer to the off position and convert pot value into motor value
  if (potValue > 10) motorValue = map(potValue, 10, 1023, 0, 255);
  // activate motor
  analogWrite(motorPin, motorValue);

  // load the motor value into payload
  payload.val = motorValue;
  // send the payload
  SimbleeCOM.send((char*)&payload, sizeof(payload));
  digitalWrite(ledPinBlue, HIGH);
  delay(100);
  digitalWrite(ledPinBlue, LOW);

  if (DEBUG) Serial.print(payload.val);

  // check the battery voltage
  if (millis() - battCheckStart > battCheckInterval) {
    int voltValue = analogRead(voltPin);
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

    if (DEBUG) Serial.print("    ");
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

  if (DEBUG) Serial.println("");

  // Ultra Low Power Delay in milliseconds - does this work?
  Simblee_ULPDelay(200);
}



