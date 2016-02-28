#ifndef DRV8830_h
#define DRV8830_h

#include "mbed.h"

// I2C support constants
#define I2C_READ    0x01 // I2C read bit set
// Some values we'll load into TWCR a lot
#define START_COND  0xA4 // (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)
#define STOP_COND   0x94 // (1<<TWINT) | (1<<TWSTO) | (1<<TWEN)
#define CLEAR_TWINT 0x84 // (1<<TWINT) | (1<<TWEN)
#define NEXT_BYTE   0xC4 // (1<<TWINT) | (1<<TWEA) | (1<<TWEN)

// Fault constants
#define FAULT 0x01
#define ILIMIT 0x10
#define OTS 0x08
#define UVLO 0x04
#define OCP 0x02

class DRV8830
{
  public:
    DRV8830(int addr);
    void drive(int speed, int address);
    void stop(int address);
    void brake(int address);
    char getFault();
  private:
    //void I2CWriteBytes(byte addr, byte *buffer, byte len);
    //void I2CReadBytes(byte addr, byte *buffer, byte len);
    int _addr;
};

#endif