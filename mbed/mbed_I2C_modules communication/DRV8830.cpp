/*
TMP102::TMP102(PinName sda, PinName scl, int addr) : m_i2c(sda, scl), m_addr(addr)
{

}

TMP102::~TMP102()
{

}
*/
#include "DRV8830.h"

I2C i2c(p28, p27);

// The address of the part is set by a jumper on the board. 
//  See datasheet or schematic for details; default is 0xD0.
DRV8830::DRV8830(int addr)
{
  _addr = addr; 
  // This sets the bit rate of the bus; I want 100kHz. See the
  //  datasheet for details on how I came up with this value.
  //TWBR = 72;
}

// Return the fault status of the DRV8830 chip. Also clears any existing faults.
char DRV8830::getFault()
{
  char buffer = 0x0;
  char clearFault = 0x80;
  // Read from an I2C slave. 
  //read (int address, char *data, int length, bool repeated=false) 
  i2c.read(0x01, &buffer, 1);
  // Write to an I2C slave. 
  // write (int address, const char *data, int length, bool repeated=false) 
  i2c.write(0x01, &clearFault, 1);
  return buffer;
}

// Send the drive command over I2C to the DRV8830 chip. Bits 7:2 are the speed
//  setting; range is 0-63. Bits 1:0 are the mode setting:
//  - 00 = HI-Z
//  - 01 = Reverse
//  - 10 = Forward
//  - 11 = H-H (brake)
void DRV8830::drive(int speed, int address)
{
  //byte faultSatusRegisterValue = 0x80;             // Before we do anything, we'll want to
  char cmd[2];
  cmd[0] = 0x00;
  //cmd[1] = 0x25;
  
  char faultSatusRegisterValue = 0x80;
                                    //  clear the fault status. To do that
                                    //  write 0x80 to register 0x01 on the
                                    //  DRV8830.
  //I2CWriteBytes(0x01, &regValue, 1); // Clear the fault status.
  // m_i2c.write(0x01, &tempRegAddr, 1); // Clear the fault status.
   //i2c.write(0x01, &faultSatusRegisterValue, 1); // Clear the fault status.
   
  faultSatusRegisterValue = (char)abs(speed);      // Find the byte-ish abs value of the input
  if (faultSatusRegisterValue > 63){
       faultSatusRegisterValue = 63; // Cap the value at 63.
  }
  faultSatusRegisterValue = faultSatusRegisterValue<<2;           // Left shift to make room for bits 1:0
  if (speed < 0){
       faultSatusRegisterValue |= 0x01;  // Set bits 1:0 based on sign of input.
  }
  else{
       faultSatusRegisterValue |= 0x02;
  }
  
  //i2c.write(0x00, &faultSatusRegisterValue, 1);  
  //i2c.write(address, &faultSatusRegisterValue, 1);  
  cmd[1] = faultSatusRegisterValue;  
  i2c.write(address, cmd, 2);  
}

// Coast to a stop by hi-z'ing the drivers.
void DRV8830::stop(int address)
{
  char cmd[2];
  cmd[0] = 0x00;
  cmd[1] = 0x80;                // See above for bit 1:0 explanation. 
  i2c.write(address, cmd, 2); 
}

// Stop the motor by providing a heavy load on it.
void DRV8830::brake(int address)
{
  char cmd[2];
  cmd[0] = 0x00;
  cmd[1] = 0x80 | 0x03;                // See above for bit 1:0 explanation. 
  //faultSatusRegisterValue |= 0x02;
  i2c.write(address, cmd, 2); 
}