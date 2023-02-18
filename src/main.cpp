#include <Arduino.h>
#include "EEPROMProgrammer.h"

#define EMPTY 0

#define HLT 0b1000000000000000
#define MI  0b0100000000000000
#define RI  0b0010000000000000
#define RO  0b0001000000000000
#define IO  0b0000100000000000
#define II  0b0000010000000000
#define AI  0b0000001000000000
#define AO  0b0000000100000000

#define EO  0b0000000010000000
#define SU  0b0000000001000000
#define BI  0b0000000000100000
#define DI  0b0000000000010000
#define CE  0b0000000000001000
#define CO  0b0000000000000100
#define J   0b0000000000000010
#define FI  0b0000000000000001

// flag sets
#define FLG_Z0C0 0
#define FLG_Z0C1 1
#define FLG_Z1C0 2
#define FLG_Z1C1 3

// commands[instruction][word];
const PROGMEM uint16_t UCODE_TEMPLATE[16][8] = {
  { MI|CO, RO|II|CE,  0,      0,      0,            0, 0, 0 },  // 0000 - NOOP
  { MI|CO, RO|II|CE,  IO|MI,  RO|AI,  0,            0, 0, 0 },  // 0001 - LDA
  { MI|CO, RO|II|CE,  IO|MI,  RO|BI,  EO|AI|FI,     0, 0, 0 },  // 0010 - ADD
  { MI|CO, RO|II|CE,  IO|MI,  RO|BI,  EO|AI|SU|FI,  0, 0, 0 },  // 0011 - SUB 
  { MI|CO, RO|II|CE,  IO|MI,  AO|RI,  0,            0, 0, 0 },  // 0100 - STA
  { MI|CO, RO|II|CE,  IO|AI,  0,      0,            0, 0, 0 },  // 0101 - LDI
  { MI|CO, RO|II|CE,  IO|J,   0,      0,            0, 0, 0 },  // 0110 - JMP
  { MI|CO, RO|II|CE,  0,      0,      0,            0, 0, 0 },  // 0111 - NOOP
  { MI|CO, RO|II|CE,  0,      0,      0,            0, 0, 0 },  // 1000 - NOOP
  { MI|CO, RO|II|CE,  0,      0,      0,            0, 0, 0 },  // 1001 - NOOP
  { MI|CO, RO|II|CE,  0,      0,      0,            0, 0, 0 },  // 1010 - NOOP
  { MI|CO, RO|II|CE,  0,      0,      0,            0, 0, 0 },  // 1011 - NOOP
  { MI|CO, RO|II|CE,  0,      0,      0,            0, 0, 0 },  // 1100 - NOOP
  { MI|CO, RO|II|CE,  0,      0,      0,            0, 0, 0 },  // 1101 - NOOP
  { MI|CO, RO|II|CE,  AO|DI,  0,      0,            0, 0, 0 },  // 1110 - OUT
  { MI|CO, RO|II|CE,  HLT,    0,      0,            0, 0, 0 }   // 1111 - HLT
};

// command variants on [flag][instruction][step]
uint16_t ucode[4][16][8];

void initUcode() {
  // ZF = 0; CF = 0;
  memcpy_P(ucode[FLG_Z0C0], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  // ZF = 0; CF = 1;
  memcpy_P(ucode[FLG_Z0C1], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  // ZF = 1; CF = 0;
  memcpy_P(ucode[FLG_Z1C0], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  // ZF = 1; CF = 1;
  memcpy_P(ucode[FLG_Z1C1], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
}


/* 
FLAG WEIRDNESS
Different flags mean we must modify the control words that come after the flag is set.
This means we can't write everything out as a clean constant.
*/


EEPROMProgrammer eeprom;


void setup() {
  Serial.begin(57600);
  // initUcode();
  // eeprom.erase(0x00);


  Serial.println("Programming EEPROM...");
  for (int address = 0; address < 1024; address += 1) {
    int flags    = (address & 0b1100000000) >> 8;
    int byte_sel = (address & 0b0010000000) >> 7; // which eeprom?
    int ins      = (address & 0b0001111000) >> 3;
    int step     = (address & 0b0000000111);

    if (byte_sel) {
      eeprom.writeByte(address, ucode[flags][ins][step]);
    } else {
      eeprom.writeByte(address, ucode[flags][ins][step] >> 8);
    }

    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
 
  Serial.println(" done");

  eeprom.printContents(0, 1024);
}



void loop() {}

