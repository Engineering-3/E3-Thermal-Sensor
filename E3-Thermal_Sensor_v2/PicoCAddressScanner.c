// Walk through all addresses, looking for any replies.
// Print out any addresses that have thermal sensors on them.
// We will read EEPROM register 0x00, cause that seems like
// it should be pretty common
int data;
int x;
int sa;

for (x = 0x01; x <= 0x7F; x++)
{
  // data = readi2c(x, 0x00);     // The normal version
  data = readi2crs(x, 0x00);      // The repeated start version

  if (data >= 0)
  {
    printf("Found a something at address 0x%02x ", x);
  
    sa = readi2c3rs(x, 0x2E); // Read SA address from Thermal Sensor
  
    if (sa > 0)
    {
      // Have we found a thermal sensor?
      if ((sa & 0xFF0000) >> 16 == x)
      {
        printf(" Thermal Sensor");
      }
    }
    printf("\n");
  }
}

printf("Done\n");
exit(1);