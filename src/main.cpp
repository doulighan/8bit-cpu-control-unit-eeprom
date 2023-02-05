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
#define FF  0b0000000000000001

#define BLOCK_SIZE 128


uint16_t commands[] = {
  MI|CO, RO|II|CE,  0,      0,      0,         0, 0, 0, // 0000 - NOOP
  MI|CO, RO|II|CE,  IO|MI,  RO|AI,  0,         0, 0, 0, // 0001 - LDA
  MI|CO, RO|II|CE,  IO|MI,  RO|BI,  EO|AI,     0, 0, 0, // 0010 - ADD
  MI|CO, RO|II|CE,  IO|MI,  RO|BI,  EO|AI|SU,  0, 0, 0, // 0011 - SUB 
  MI|CO, RO|II|CE,  IO|MI,  AO|RI,  0,         0, 0, 0, // 0100 - STA
  MI|CO, RO|II|CE,  IO|AI,  0,      0,         0, 0, 0, // 0101 - LDI (A)
  MI|CO, RO|II|CE,  IO|J,   0,      0,         0, 0, 0, // 0110 - JMP
  MI|CO, RO|II|CE,  0,      0,      0,         0, 0, 0, // 0111 - NOOP
  MI|CO, RO|II|CE,  0,      0,      0,         0, 0, 0, // 1000 - NOOP
  MI|CO, RO|II|CE,  0,      0,      0,         0, 0, 0, // 1001 - NOOP
  MI|CO, RO|II|CE,  0,      0,      0,         0, 0, 0, // 1010 - NOOP
  MI|CO, RO|II|CE,  0,      0,      0,         0, 0, 0, // 1011 - NOOP
  MI|CO, RO|II|CE,  0,      0,      0,         0, 0, 0, // 1100 - NOOP
  MI|CO, RO|II|CE,  0,      0,      0,         0, 0, 0, // 1101 - NOOP
  MI|CO, RO|II|CE,  AO|DI,  0,      0,         0, 0, 0, // 1110 - OUT
  MI|CO, RO|II|CE,  HLT,    0,      0,         0, 0, 0  // 1111 - HLT
};


/* 
We're programming the first half of each instruction word in the first block of 128,
and the second half in the second block. Then, the left eeporm will use block 1, 
and the right eeprom will use block 2. We can program them exactly the same.
*/


EEPROMProgrammer eeprom;

void writeControl(int instruction, int step, int data) {
  int address = (instruction << 3) + step;
  eeprom.writeByte(address, (byte) data);  
}

void setup() {
  Serial.begin(57600);
  // eeprom.erase(0x00);

  Serial.println("Writing signals...");

 
  for (int address = 0; address < sizeof(commands)/sizeof(commands[0]); address += 1) {
    eeprom.writeByte(address, commands[address] >> 8); // block 1
    eeprom.writeByte(address+BLOCK_SIZE, commands[address]); // block 2
  }
 
  Serial.println(" done");

  eeprom.printContents(256 * 1);
}



void loop() {}

