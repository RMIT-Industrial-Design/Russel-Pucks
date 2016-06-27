/*
  Russel Master
  by Alexander Radevski
  Scott Mitchell
  RMIT University

  Started 10/06/16

  2.1
  - code shared with Scott.
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
struct payloadStructure {
  int16_t Switch;
} __attribute__((packed)); // hint to complier to pack into memory


  int switch1 = 3;
  int switch2 = 5;
  int switch3 = 6;
  int switch4 = 9;
  int switch5 = 11;
  int switch6 = 12;
  

void setup() {
  // put your setup code here, to run once:
pinMode(switch1, INPUT);
pinMode(switch2, INPUT);
pinMode(switch3, INPUT);
pinMode(switch4, INPUT);
pinMode(switch5, INPUT);
pinMode(switch6, INPUT);


}

void loop() {
  // put your main code here, to run repeatedly:

}

void SimbleeCOM_onReceive(unsigned int esn, const char *payload, int len, int rssi) {
memcpy(&received, payload, len);
rx_flag = true
}
