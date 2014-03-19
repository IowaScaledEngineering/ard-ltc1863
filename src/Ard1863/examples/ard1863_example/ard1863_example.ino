#include <SPI.h>

/*************************************************************************
Title:    ARD-LTC186X Library Example Arduino Sketch
Authors:  Nathan D. Holmes <maverick@drgw.net>
File:     $Id: $
License:  GNU General Public License v3

LICENSE:
    Copyright (C) 2014 Nathan D. Holmes & Michael D. Petersen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*************************************************************************/

#include <Wire.h>
#include <Ard1863.h>

Ard186x ard186xboard1;

byte confChan=0;

void setup() {
  byte retval = 0;

  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
//  Wire.begin();
  SPI.begin();  
  ard186xboard1.begin(DEVICE_LTC1863, ARD186X_EEP_ADDR_ZZ);
  ard186xboard1.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_0P, 1);

  Serial.print("eeprom mac = [");
  Serial.print(ard186xboard1.eui48Get());
  Serial.print("]\n");
  
  Serial.print(" write 42 to eeprom[0] ");
  retval = ard186xboard1.eepromWrite(0, 42, true);
  Serial.print(" retval=");
  Serial.print(retval);
  Serial.print("\n");

  Serial.print("read eeprom[0] ");
  Serial.print(ard186xboard1.eepromRead(0, true));
  Serial.print("\n");

}


void loop() {
  // print the results to the serial monitor:

  byte retval = 0;

  Serial.print("Channel ");
  Serial.print(confChan);
  Serial.print(" Single Ended");
  Serial.print(" = [");
  Serial.print(ard186xboard1.ltc186xRead());
  Serial.print("]\n");
  
  confChan++;
  if (confChan >= 8)
  {
     Serial.print("\n");
     confChan = 0;
  }     
  switch(confChan)
  {
    case 0:
      ard186xboard1.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_0P, 1);
      break;
    
    case 1:
      ard186xboard1.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_1P, 1);
      break;
    
    case 2:
      ard186xboard1.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_2P, 1);
      break;

    case 3:
      ard186xboard1.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_3P, 1);
      break;
    
    case 4:
      ard186xboard1.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_4P, 1);
      break;
    
    case 5:
      ard186xboard1.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_5P, 1);
      break;
    
    case 6:
      ard186xboard1.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_6P, 1);
      break;
    
    case 7:
      ard186xboard1.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_7P, 1);
      break;
  }
    
  
  delay(500);
}
