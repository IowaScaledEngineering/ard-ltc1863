/*************************************************************************
Title:    ARD-LTC186X Library Example Arduino Sketch
Authors:  Nathan D. Holmes <maverick@drgw.net>, Michael Petersen <railfan@drgw.net>
File:     $Id: $
License:  GNU General Public License v3

LICENSE:
    Copyright (C) 2018 Nathan D. Holmes & Michael D. Petersen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Ard1863.h>


// This little bit of code redefines what the serial console is called 
// on the Arduino M0 family.  It's called SerialUSB there and Serial on all
// the rest of the Arduinos.  That's rational, right? :(
// Pretty much every other Arduino calls it just "Serial", so default to that
#define SerialConsole Serial

// The serial console is called SerialUSB on the M0 - uncomment this if needed
//#define SerialConsole SerialUSB


Ard186x ard186xboard1;

byte confChan=0;

void setup() {
  byte retval = 0;
  
  int chipSelectPin = 3; // 3 is good for most things, but Leonardos share SCL with D3.  8 is the other option
  
  // initialize serial communications at 9600 bps:
  SerialConsole.begin(9600);

  while(!SerialConsole);

  // This is the default initialization command
  ard186xboard1.begin(DEVICE_LTC1863, ARD186X_EEP_ADDR_ZZ, chipSelectPin);
  
  // For compatibility, the library resets the SPI mode every time it wants to use the SPI bus.
  // However, if the ARD-LTC186x is the only thing using the SPI bus, you don't need to run the 
  // heavy and slow SPI begin/end transaction calls.  Calling setFastSPI with an argument of 1
  // turns these slow calls off and gives you maximum speed.  Uncomment out the command below
  // to try it.  It's about a 100% speed boost on the ARMs, 10% on AVRs
  // ard186xboard1.setFastSPI(1);
  
  ard186xboard1.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_0P, 1);

  SerialConsole.print("eeprom mac = [");
  SerialConsole.print(ard186xboard1.eui48Get());
  SerialConsole.print("]\n");
  
  SerialConsole.print(" write 42 to eeprom[0] ");
  retval = ard186xboard1.eepromWrite(0, 42, true);
  SerialConsole.print(" retval=");
  SerialConsole.print(retval);
  SerialConsole.print("\n");

  SerialConsole.print("read eeprom[0] ");
  SerialConsole.print(ard186xboard1.eepromRead(0, true));
  SerialConsole.print("\n");

}

byte loopCounter = 0;

void speedProfile()
{
  unsigned int k = 0;
  unsigned long startTime, endTime;

  ard186xboard1.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_0P, 1);

  SerialConsole.print("Starting speed profile on channel 0\n");

  startTime = millis();
  for(unsigned long i=0; i<100000; i++)
  {
    k += ard186xboard1.ltc186xRead();
  }
  endTime = millis();

  SerialConsole.print("Max data rate ");
  SerialConsole.print(100000UL / (endTime-startTime));
  SerialConsole.print(" ksps\n");
}

void loop() {
  // print the results to the serial monitor:

  byte retval = 0;


  SerialConsole.print("Channel ");
  SerialConsole.print(confChan);
  SerialConsole.print(" Single Ended");
  SerialConsole.print(" = [");
  SerialConsole.print(ard186xboard1.ltc186xRead());
  SerialConsole.print("]\n");
  
  confChan++;
  if (confChan >= 8)
  {
     SerialConsole.print("\n");
     confChan = 0;

     if (0 == loopCounter)
     {
       // Every 28 loops, run the speed profiler
       speedProfile();
     }
     loopCounter = (loopCounter + 1) % 4;
     
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
