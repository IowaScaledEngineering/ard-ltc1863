/*************************************************************************
Title:    ARD-LTC2499 Driver Library
Authors:  Nathan D. Holmes <maverick@drgw.net>
File:     $Id: $
License:  GNU General Public License v3

LICENSE:
    Copyright (C) 2013 Nathan D. Holmes & Michael D. Petersen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#include "Ard1863.h"

void Ard186x::ltc186xSleep()
{
	current186xConfig |= _BV(LTC186X_CONFIG_SLP);
	digitalWrite(ARD186X_SS_DIO, LOW);
	SPI.transfer(current186xConfig);
	SPI.transfer(0);
	digitalWrite(ARD186X_SS_DIO, HIGH);
}

void Ard186x::ltc186xWake()
{
	uint8_t wasSleeping = current186xConfig & _BV(LTC186X_CONFIG_SLP);
	current186xConfig &= ~_BV(LTC186X_CONFIG_SLP);
	digitalWrite(ARD186X_SS_DIO, LOW);
	SPI.transfer(current186xConfig);
	SPI.transfer(0);
	digitalWrite(ARD186X_SS_DIO, HIGH);

	// Wake-up time if we were really sleeping
	if (wasSleeping)
		_delay_ms(70);
}

unsigned int Ard186x::ltc186xRead()
{
	uint16_t retval = 0;
	digitalWrite(ARD186X_SS_DIO, LOW);
	if (DEVICE_LTC1863 == ltc186xDeviceType)
	{
		retval = SPI.transfer(current186xConfig);
		retval <<= 4;
		retval |= 0x0F & (SPI.transfer(0)>>4);
	}
	else if (DEVICE_LTC1867 == ltc186xDeviceType)
	{
		retval = SPI.transfer(current186xConfig);
		retval <<= 8;
		retval |= 0xFF & SPI.transfer(0);
	}

	digitalWrite(ARD186X_SS_DIO, HIGH);
	return(retval);
}

int Ard186x::ltc186xReadBipolar()
{
	int retval = (int)ltc186xRead();
	
	if (DEVICE_LTC1863 == ltc186xDeviceType
		&& (retval & 0x0800))
		retval |= 0xF000;
	
	return(retval);
}

void Ard186x::ltc186xChangeChannel(byte nextChannel, byte unipolar=1)
{
	internalChangeChannel(nextChannel, unipolar);
	digitalWrite(ARD186X_SS_DIO, LOW);
	SPI.transfer(current186xConfig);
	SPI.transfer(0);
	digitalWrite(ARD186X_SS_DIO, HIGH);	
}

void Ard186x::internalChangeChannel(byte nextChannel, byte unipolar)
{
	current186xConfig = unipolar?_BV(LTC186X_CONFIG_UNI):0;
	current186xConfig |= nextChannel 
		& (_BV(LTC186X_CONFIG_SINGLE_END) 
			| _BV(LTC186X_CONFIG_ODD) 
			| _BV(LTC186X_CONFIG_S1) 
			| _BV(LTC186X_CONFIG_S0)
			| _BV(LTC186X_CONFIG_COM));
}

unsigned int Ard186x::ltc186xReadAndChangeChannel(byte nextChannel, byte unipolar=1)
{
	ltc186xChangeChannel(nextChannel, unipolar);
	return(ltc186xRead());
}

int Ard186x::ltc186xReadBipolarAndChangeChannel(byte nextChannel, byte unipolar=0)
{
	ltc186xChangeChannel(nextChannel, unipolar);
	return(ltc186xReadBipolar());
}


Ard186x::Ard186x()
{
	init_status = ARD186X_LTC186X_ERR | ARD186X_EEPROM_ERR;
	ltc186xDeviceType = 0;
	i2cAddr_eeprom = 0;
	current186xConfig = LTC186X_CHAN_DIFF_0P_1N;
	strcpy(eui48, "Unknown");
}

const char* Ard186x::eui48Get()
{
	return(eui48);
}


byte Ard186x::begin(byte deviceType, byte eepromAddress)
{
	byte retval = 0;
	byte i;

	pinMode(ARD186X_SS_DIO, OUTPUT);
	digitalWrite(ARD186X_SS_DIO, HIGH);

	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	
	init_status = ARD186X_SUCCESS;
	strcpy(eui48, "Unknown");

	if (DEVICE_LTC1863 == deviceType || DEVICE_LTC1867 == deviceType)
		ltc186xDeviceType = deviceType;
	else
		init_status |= ARD186X_LTC186X_ERR;

	current186xConfig = LTC186X_CHAN_DIFF_0P_1N;

	digitalWrite(ARD186X_SS_DIO, LOW);
	SPI.transfer(current186xConfig);
	SPI.transfer(0);
	digitalWrite(ARD186X_SS_DIO, HIGH);

/*	i2cAddr_eeprom = eepromAddress;
	Wire.beginTransmission(i2cAddr_eeprom);
	Wire.write(ARD186X_EEPROM_ADDR_EUI48);
	retval = Wire.endTransmission(false);
	// Anything but zero means we couldn't initialize the LTC2499
	if (0 != retval)
	{
		// Make sure we send a stop bit
		Wire.endTransmission(true);
		i2cAddr_eeprom = 0;
		init_status |= ARD186X_EEPROM_ERR;
	}
	else
	{

		Wire.requestFrom(i2cAddr_eeprom, (uint8_t)6, (uint8_t)true);
		if (Wire.available() < 6)
		{
			i2cAddr_eeprom = 0;		
			init_status |= ARD186X_EEPROM_ERR;
		} else {
			memset(eui48, 0, sizeof(eui48));
			for(i=0; i<12; i+=2)
				sprintf(&eui48[i], "%02X", Wire.read());
		}
	}		*/
	return(init_status);
}

byte Ard186x::eepromRead(int address, byte defaultOnError=0)
{
	byte retval = 0;
	/*
	// Our address range only goes to 0xFF
	if (address > 0xFF)
		return(defaultOnError);
	
	Wire.beginTransmission(i2cAddr_eeprom);
	Wire.write((uint8_t)address);
	retval = Wire.endTransmission(false);
	// Anything but zero means we couldn't initialize the LTC2499
	if (0 != retval)
	{
		// Make sure we send a stop bit
		Wire.endTransmission(true);
		return(0);
	}

	Wire.requestFrom((uint8_t)i2cAddr_eeprom, (uint8_t)1, (uint8_t)true);
	if (Wire.available() < 1)
		return(defaultOnError);
	return(Wire.read());*/
	return(0);
}

byte Ard186x::eepromWrite(int address, byte value, byte blocking=1)
{
/*	uint8_t waitLoop = 10;
	byte retval = 0;
	if (0 == i2cAddr_eeprom)
		return(ARD186X_EEPROM_ERR);

	// Our writable address range only goes to 0xFF
	if (address > 0x7F)
		return(ARD186X_EEPROM_ERR);
		
	Wire.beginTransmission(i2cAddr_eeprom);
	Wire.write(address);
	Wire.write(value);
	retval = Wire.endTransmission(true);
	// Anything but zero means we couldn't write to the LTC2499
	if (0 != retval)
	{
		return(ARD186X_EEPROM_ERR);
	}	
	
	if (0 != blocking)
	{
		while (0 != waitLoop--)
		{
			Wire.beginTransmission(i2cAddr_eeprom);
			Wire.write(address);
			if (0 == Wire.endTransmission(true))
				return(0);
			_delay_ms(1);
		}
		return(ARD186X_EEPROM_ERR);
	}
	return(ARD186X_SUCCESS);*/
	return(0);
}


