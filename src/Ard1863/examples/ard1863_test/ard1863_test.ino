/*************************************************************************
Title:    ARD-LTC1863 Board Test
Authors:  Michael Petersen <railfan@drgw.net>
File:     $Id: $
License:  GNU General Public License v3

LICENSE:
    Copyright (C) 2015 Nathan D. Holmes & Michael D. Petersen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*************************************************************************/

// Instructions:
//   Short channel under test to VREF
//   Short adjacent channel to GND (1k works)

#include <Wire.h>
#include <SPI.h>
#include <Ard1863.h>

Ard186x ard186xboard1;

TwoWire* WireInterface;

byte confChan=0;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(115200);
  
#ifdef ARDUINO_SAM_DUE
	WireInterface = &Wire1;
#else 
	WireInterface = &Wire;
#endif
  
  WireInterface->begin();
//  ard1863board1.begin(DEVICE_LTC1863, ARD186X_EEP_ADDR_ZZ);
  ard186xboard1.begin(DEVICE_LTC1867, ARD186X_EEP_ADDR_ZZ, 3);
  ard186xboard1.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_0P, 1);

  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
}
byte i=0;

void loop() {
  byte retval = 0;
 
//  goto skipJumpers;

  Serial.print("Looking for 0x52 (JP4)... ");
  findI2CSlave(0x52);

  Serial.print("Looking for 0x51 (JP5)... ");
  findI2CSlave(0x51);

  Serial.print("Looking for 0x53 (Open)... ");
  findI2CSlave(0x53);

  Serial.print("eeprom mac = [");
  Serial.print(ard186xboard1.eui48Get());
  Serial.print("]\n");
  
  Serial.print("write eeprom[0] 0x");
  Serial.print(0xAA, HEX);
  retval = ard186xboard1.eepromWrite(0, 0xAA, true);
  Serial.print(" retval=");
  Serial.print(retval);
  Serial.print("\n");

  Serial.print("read eeprom[0] 0x");
  Serial.print(ard186xboard1.eepromRead(0, true), HEX);
  Serial.print("\n");

  Serial.print("write eeprom[0] 0x");
  Serial.print(0x55, HEX);
  retval = ard186xboard1.eepromWrite(0, 0x55, true);
  Serial.print(" retval=");
  Serial.print(retval);
  Serial.print("\n");

  Serial.print("read eeprom[0] 0x");
  Serial.print(ard186xboard1.eepromRead(0, true), HEX);
  Serial.print("\n");

skipJumpers:

  for(i=0; i<8; i++)
  {
    byte newChannel;
    byte adjacentChannel;
    
    switch(i)
    {
       case 0:
          newChannel = LTC186X_CHAN_SINGLE_0P;
          adjacentChannel = LTC186X_CHAN_SINGLE_1P;
          break;
          
       case 1:
          newChannel = LTC186X_CHAN_SINGLE_1P;
          adjacentChannel = LTC186X_CHAN_SINGLE_2P;
          break;

       case 2:
          newChannel = LTC186X_CHAN_SINGLE_2P;
          adjacentChannel = LTC186X_CHAN_SINGLE_3P;
          break;

       case 3:
          newChannel = LTC186X_CHAN_SINGLE_3P;
          adjacentChannel = LTC186X_CHAN_SINGLE_4P;
          break;

       case 4:
          newChannel = LTC186X_CHAN_SINGLE_4P;
          adjacentChannel = LTC186X_CHAN_SINGLE_5P;
          break;
          
       case 5:
          newChannel = LTC186X_CHAN_SINGLE_5P;
          adjacentChannel = LTC186X_CHAN_SINGLE_6P;
          break;

       case 6:
          newChannel = LTC186X_CHAN_SINGLE_6P;
          adjacentChannel = LTC186X_CHAN_SINGLE_7P;
          break;

       case 7:
          newChannel = LTC186X_CHAN_SINGLE_7P;
          adjacentChannel = LTC186X_CHAN_SINGLE_0P;  // Bogus
          break;
    }

    long adc = 0;
    ard186xboard1.ltc186xChangeChannel(newChannel, 1);

    uint8_t count = 0;
    Serial.print("Channel ");
    Serial.print(i);
    do
    {
      adc = ard186xboard1.ltc186xRead();
//      Serial.println(adc);
      delay(100);
      if((38000 < adc) && (adc < 42000))
      {
        count++;
        Serial.print("+");
      }
      else
      {
        count = 0;
        Serial.print(".");
      }
    } while(count < 3);

    // Check that it's not shorted to the adjacent channel
    ard186xboard1.ltc186xChangeChannel(adjacentChannel, 1);

    count = 0;
    do
    {
      adc = ard186xboard1.ltc186xRead();
//      Serial.println(adc);
      delay(250);
      if((adc < 38000) || (42000 < adc))
      {
        count++;
        Serial.print("|");
      }
      else
      {
        count = 0;
        Serial.print("-");
      }
    } while(count < 3);

    // Verify it's still shorted to original channel
    ard186xboard1.ltc186xChangeChannel(newChannel, 1);

    count = 0;
    do
    {
      adc = ard186xboard1.ltc186xRead();
//      Serial.println(adc);
      delay(250);
      if((38000 < adc) && (adc < 42000))
      {
        count++;
        Serial.print("+");
      }
      else
      {
        count = 0;
        Serial.print(".");
      }
    } while(count < 1);

    Serial.println(" Done!");
  }
}

void findI2CSlave(uint8_t addr)
{
  byte stat, done = 0;
  while(!done)
  {
    WireInterface->beginTransmission(addr);
    stat = WireInterface->endTransmission();
    if(!stat)
    {
      Serial.print("\a");
      Serial.println("Done!");
      done = 1;
    }
  }
  delay(1000);
}

byte enumerateI2C(boolean showErrors)
{
  byte addr, stat, last_ack = 0x00;
 
  Serial.print("Enumerating all I2C devices\n (Press any key to stop scanning I2C bus)\n-------------\n");
 
  for(addr=0x00; addr<0x7F; addr++)
  {
    WireInterface->beginTransmission(addr);
    stat = WireInterface->endTransmission();
    if(stat)
    {
      if(showErrors)
      {
        Serial.print("0x");
        Serial.print(addr, HEX);
        Serial.print(": ");
        Serial.print(stat);
        Serial.print("\n");
      }
    }  
    else
    {   
      Serial.print("0x");
      Serial.print(addr, HEX);
      Serial.print(" ");
      last_ack = addr;   
    }
  }  
  Serial.print("\n");
  return last_ack;
}
 
