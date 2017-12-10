// This PicoC program will attempt to change the I2C (SMBus) address of a 
// thermal sensor from ADR_START to ADR_END. You must set up these two
// defines (immediately below this comment) properly. The ADR_START value
// must be the sensor's current address, and the ADR_END is what the new
// address is that you want it to move to. If you don't know what
// address your sensor is currently at, you can use the PicoCAddressScanner.c
// program to find it.
// This code will check to make sure it finds a sensor at ADR_START, then
// it will change the address,
// For a standard RCMx1 based system, don't use
// addresses 0x1c or 0x30, since those are already used by devices on the 
// I2C bus.
// If any failures are found during the process, these will be printed out
// and the process stopped.
// NOTE: After you have run this program (with no errors) you must reboot
// the sensor for it to appear on the new address.

#define ADR_START 0x5A
#define ADR_END   0x5F

// Global variables
int data;
unsigned char pec;

// Compute the CRC-8 on a single byte
unsigned char Crc8( unsigned char inCrc, unsigned char inData )
{
  unsigned char   i;
  unsigned char   data;

  data = inCrc ^ inData;

  for ( i = 0; i < 8; i++ )
  {
    if (( data & 0x80 ) != 0 )
    {
      data <<= 1;
      data ^= 0x07;
    }
    else
    {
      data <<= 1;
    }
  }
  
  return data;
}

// Compute the CRC-8 on a full 'write' transaction to the thermal sensor. Note that this
// function assumes a 3-byte write (writei2c3), and is tailored to the known packet
// structure of this type of transaction. It returns the 8-bit PEC result.
unsigned char Crc8Block(unsigned char address, unsigned char reg, unsigned int reg_value)
{
  unsigned char crc = 0;
  
  crc = Crc8(crc, (address << 1));
  crc = Crc8(crc, reg);
  crc = Crc8(crc, (reg_value & 0x000000FF));
  crc = Crc8(crc, (reg_value & 0x0000FF00) >> 8);
  
  return crc;
}

////////////////////////
// Main program flow  //
////////////////////////

// First check to see that the sensor is present on the expected address
data = readi2crs(ADR_START, 0x00);

if (data < 0)
{
  printf("ERROR: Sensor not found on START address of 0x%02x\n.", ADR_START);
}
else
{
  // We now know we have a sensor on ADR_START. First thing is to zero out its EEPROM location 0x0E
  pec = Crc8Block(ADR_START, 0x2E, 0x0000);
  data = writei2c3(ADR_START, 0x2E, ((unsigned int)pec << 16) | 0x0000);
  
  if (data < 0)
  {
    printf("ERROR: Failure to zero address of 0x%02x\n", ADR_START);
  }
  else
  {
    delay(10); // Wait for EEPROM to finish erasing

    // Write new value to location 0x0E (I2C address)
    pec = Crc8Block(ADR_START, 0x2E, ADR_END);
    data = writei2c3(ADR_START, 0x2E, ((unsigned int)pec << 16) | ADR_END);
    
    if (data < 0)
    {
      printf("ERROR: Failure to write new address of 0x%02x\n", ADR_END);
    }
    else
    {
      printf("Sensor properly moved from 0x%02x to 0x%02x\n", ADR_START, ADR_END);
      printf("Reboot sensor for it to appear at new address\n");
    }
  }
}

printf("Done\n");
exit(1);
