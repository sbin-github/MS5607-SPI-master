#include "MS5607.h"
#include <SPI.h>

#define ADDR 15   // CS (CSB) pin when using SPI (Arduino numbering)

// MS5607(CS_PIN, INV_CS_SIGNAL)
MS5607 MS5607(ADDR, 0);

void setup() {
  Serial.begin(115200);
  Serial.println();

  SPI.begin();
  MS5607.init();

  unsigned int C_read[8];

  uint8_t i;
  unsigned int n_rem = 0;
  unsigned int crc_read;
  unsigned char n_bit;


  // Calculate and check PROM's CRC4
  // 
  int result = MS5607.read();
  for (i = 0; i < 8; i++)
  {
    C_read[i] = MS5607.getPromValue(i);
    //C_read[i] = C_test[i];
  }
  crc_read = C_read[7];
  C_read[7]=(0xFF00 & (C_read[7]));
  for (i = 0; i < 16; i++)
  {
    if (i%2==1) n_rem ^= (unsigned short) ( (C_read[i>>1]) & 0x00FF );
    else n_rem ^= (unsigned short) (C_read[i>>1]>>8);
    for (n_bit = 8; n_bit > 0; n_bit--)
    {
      if (n_rem & (0x8000))
      {
        n_rem = (n_rem << 1) ^ 0x3000;
      }
      else
      {
        n_rem = (n_rem << 1);
      }
    }
  }
  n_rem= (0x000F & (n_rem >> 12));

  Serial.print ("CRC (calculated) : ");
  Serial.println (n_rem,HEX);
  Serial.print ("CRC (read)       : ");
  C_read[7]=crc_read;
  Serial.println (C_read[7] & 15,HEX);
  Serial.println();
}


void loop()
{
    uint32_t test;
    int result = MS5607.read(8);                          // 8 - 12 bit oversampling; default 8
    if (result != 0)
    {
        Serial.print("Error in read: ");
        Serial.println(result);
    }
    else
    {
        //Serial.print("\tC6:\t");
        Serial.print("\tTemp:\t");
        Serial.print(MS5607.getTemperature() * 0.01, 2);  // print as float
        Serial.print("\t\tP:\t");
        Serial.print(MS5607.getPressure() * 0.01, 2);     // print as float
        Serial.println();
    }
    delay(1000);
}
