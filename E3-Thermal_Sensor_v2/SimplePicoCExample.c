// Default I2C Address is 0x5A, RAM register with temp is 0x07
int T0;
int x;
float temp;

T0 = readi2c2rs(0x5A, 0x07);

// Flip around the two bytes of the result
x = T0 >> 8;
x = x | ((T0 & 0xFF) << 8);

if (x > 0x8000)
{
    printf("Error when reading temperature.\n");
}
else
{
    // Get temp in deg Kelvin
    temp = x/50.0;
    // Convert to deg C
    temp = temp - 273.15;
    printf("Temp =  %f C\n", temp);
}

exit(1);