#include <Arduino.h>
#include "EEPROMProgrammer.h"

#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_ENABLE 13


EEPROMProgrammer::EEPROMProgrammer()
{
    pinMode(SHIFT_DATA, OUTPUT);
    pinMode(SHIFT_CLK, OUTPUT);
    pinMode(SHIFT_LATCH, OUTPUT);
    digitalWrite(WRITE_ENABLE, HIGH);
    pinMode(WRITE_ENABLE, OUTPUT);
}

EEPROMProgrammer::~EEPROMProgrammer() {
    Serial.end();
}


void EEPROMProgrammer::setAddress(int address, bool outputEnable)
{
    // Get most sig bits, and also set the outputEnable pin to bool
    int firstWord = (address >> 8) | (outputEnable ? 0x00 : 0x80);
    shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, firstWord);
    shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

    digitalWrite(SHIFT_LATCH, LOW);
    digitalWrite(SHIFT_LATCH, HIGH);
    digitalWrite(SHIFT_LATCH, LOW);
}

byte EEPROMProgrammer::readByte(int address)
{
    for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
        pinMode(pin, INPUT);
    }
    setAddress(address, true);

    byte data = 0;
    for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
        data = (data << 1) + digitalRead(pin);
    }
    return data;
}

void EEPROMProgrammer::writeByte(int address, byte data)
{
    this->setAddress(address, false);
    for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
        pinMode(pin, OUTPUT);
    }

    for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
        digitalWrite(pin, data & 1);
        data = data >> 1;
    }

    digitalWrite(WRITE_ENABLE, LOW);
    delayMicroseconds(1);
    digitalWrite(WRITE_ENABLE, HIGH);
    delay(10);
}

void EEPROMProgrammer::writeDataSet(byte data[])
{
    Serial.print("Programming EEPROM");
    for (int address = 0; address < sizeof(data); address += 1) {
        this->writeByte(address, data[address]);
        if (address % 64 == 0) {
            Serial.print(".");
        }
    }
    Serial.println(" done");
}

void EEPROMProgrammer::erase(byte nullVal)
{
    Serial.println("Erasing EEPROM");
    for (int address = 0; address < 2048; address += 1) {
        this->writeByte(address, nullVal);
        if (address % 64 == 0) {
            Serial.print(".");
        }
    }
    Serial.println(" done");
}

void EEPROMProgrammer::printContents(int numBytes)
{
    Serial.println("Reading EEPROM");
    for (int base = 0; base < numBytes; base += 16) {

        if (base % 256 == 0) {
            Serial.println("------------------------------------------------------");
        }

        byte data[16];
        for (int offset = 0; offset < 16; offset += 1) {
            data[offset] = this->readByte(base + offset);
        }

        char buf[80];
        sprintf(
            buf,
            "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
        Serial.println(buf);
    }
    Serial.println("------------------------------------------------------");
    Serial.println("------------------------------------------------------");
}