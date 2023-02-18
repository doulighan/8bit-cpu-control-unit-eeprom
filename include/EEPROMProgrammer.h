/*
  Some helper functions to read and write data from an EEPROM, using shift data.
*/

#ifndef EEPROMPROGRAMMER_H_
#define EEPROMPROGRAMMER_H_

class EEPROMProgrammer;

class EEPROMProgrammer
{
  public:
    EEPROMProgrammer();
    ~EEPROMProgrammer();
    void setAddress(int address, bool outputEnable);
    byte readByte(int address);
    void writeByte(int address, byte data);
    void writeDataSet(byte data[]);
    void erase(byte nullVal);
    void printContents(int start, int num);
};

#endif