/*************************************************************************
Title:    ARD-LTC1863 / ARD-LTC1867 Driver Library
Authors:  Nathan D. Holmes <maverick@drgw.net>
File:     $Id: $
License:  GNU General Public License v3

LICENSE:
    Copyright (C) 2017 Nathan D. Holmes & Michael D. Petersen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*************************************************************************/
#ifndef ARD186X_H
#define ARD186X_H

#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include "Arduino.h"
#include "Wire.h"
#include "SPI.h"

#ifndef _BV
#define _BV(a) (1<<(a))
#endif

#define LTC186X_CONFIG_SINGLE_END 7
#define LTC186X_CONFIG_ODD        6
#define LTC186X_CONFIG_S1         5
#define LTC186X_CONFIG_S0         4
#define LTC186X_CONFIG_COM        3
#define LTC186X_CONFIG_UNI        2
#define LTC186X_CONFIG_SLP        1

#define LTC186X_CHAN_DIFF_0P_1N    (0)
#define LTC186X_CHAN_DIFF_2P_3N    (_BV(LTC186X_CONFIG_S0))
#define LTC186X_CHAN_DIFF_4P_5N    (_BV(LTC186X_CONFIG_S1))
#define LTC186X_CHAN_DIFF_6P_7N    (_BV(LTC186X_CONFIG_S1) | _BV(LTC186X_CONFIG_S0))
#define LTC186X_CHAN_DIFF_1P_0N    (_BV(LTC186X_CONFIG_ODD))
#define LTC186X_CHAN_DIFF_3P_2N    (_BV(LTC186X_CONFIG_ODD) | _BV(LTC186X_CONFIG_S0))
#define LTC186X_CHAN_DIFF_5P_4N    (_BV(LTC186X_CONFIG_ODD) | _BV(LTC186X_CONFIG_S1))
#define LTC186X_CHAN_DIFF_7P_6N    (_BV(LTC186X_CONFIG_ODD) | _BV(LTC186X_CONFIG_S1) | _BV(LTC186X_CONFIG_S0))

#define LTC186X_CHAN_DIFF_0P_7COM  (_BV(LTC186X_CONFIG_COM) | _BV(LTC186X_CONFIG_SINGLE_END))
#define LTC186X_CHAN_DIFF_1P_7COM  (_BV(LTC186X_CONFIG_COM) | _BV(LTC186X_CONFIG_SINGLE_END) | _BV(LTC186X_CONFIG_ODD))
#define LTC186X_CHAN_DIFF_2P_7COM  (_BV(LTC186X_CONFIG_COM) | _BV(LTC186X_CONFIG_SINGLE_END) | _BV(LTC186X_CONFIG_S0))
#define LTC186X_CHAN_DIFF_3P_7COM  (_BV(LTC186X_CONFIG_COM) | _BV(LTC186X_CONFIG_SINGLE_END) | _BV(LTC186X_CONFIG_ODD) | _BV(LTC186X_CONFIG_S0))
#define LTC186X_CHAN_DIFF_4P_7COM  (_BV(LTC186X_CONFIG_COM) | _BV(LTC186X_CONFIG_SINGLE_END) | _BV(LTC186X_CONFIG_S1))
#define LTC186X_CHAN_DIFF_5P_7COM  (_BV(LTC186X_CONFIG_COM) | _BV(LTC186X_CONFIG_SINGLE_END) | _BV(LTC186X_CONFIG_ODD) | _BV(LTC186X_CONFIG_S1))
#define LTC186X_CHAN_DIFF_6P_7COM  (_BV(LTC186X_CONFIG_COM) | _BV(LTC186X_CONFIG_SINGLE_END) | _BV(LTC186X_CONFIG_S1)  | _BV(LTC186X_CONFIG_S0))

#define LTC186X_CHAN_SINGLE_0P     (_BV(LTC186X_CONFIG_SINGLE_END))
#define LTC186X_CHAN_SINGLE_1P     (_BV(LTC186X_CONFIG_SINGLE_END) | _BV(LTC186X_CONFIG_ODD))
#define LTC186X_CHAN_SINGLE_2P     (_BV(LTC186X_CONFIG_SINGLE_END) | _BV(LTC186X_CONFIG_S0))
#define LTC186X_CHAN_SINGLE_3P     (_BV(LTC186X_CONFIG_SINGLE_END) | _BV(LTC186X_CONFIG_ODD) | _BV(LTC186X_CONFIG_S0))
#define LTC186X_CHAN_SINGLE_4P     (_BV(LTC186X_CONFIG_SINGLE_END) | _BV(LTC186X_CONFIG_S1))
#define LTC186X_CHAN_SINGLE_5P     (_BV(LTC186X_CONFIG_SINGLE_END) | _BV(LTC186X_CONFIG_ODD) | _BV(LTC186X_CONFIG_S1))
#define LTC186X_CHAN_SINGLE_6P     (_BV(LTC186X_CONFIG_SINGLE_END) | _BV(LTC186X_CONFIG_S1)  | _BV(LTC186X_CONFIG_S0))
#define LTC186X_CHAN_SINGLE_7P     (_BV(LTC186X_CONFIG_SINGLE_END) | _BV(LTC186X_CONFIG_ODD) | _BV(LTC186X_CONFIG_S1) | _BV(LTC186X_CONFIG_S0))

#define ARD186X_SUCCESS      0
#define ARD186X_LTC186X_ERR  1
#define ARD186X_EEPROM_ERR   2

#define ARD186X_EEPROM_ADDR_EUI48  0xFA

#define ARD186X_EEP_DISABLE 0x00
#define ARD186X_EEP_ADDR_00 0x50
#define ARD186X_EEP_ADDR_0Z 0x51
#define ARD186X_EEP_ADDR_Z0 0x52
#define ARD186X_EEP_ADDR_ZZ 0x53

#define DEVICE_LTC1863   0
#define DEVICE_LTC1867   1

#define ARD186X_SS_DIO   3

#define ARD186X_SKIP_SPI_TRANSACTION 0x01

class Ard186x
{
	public:
		Ard186x();
		byte begin(byte deviceType, byte eepromAddress, int cs_pin);

		const char* eui48Get();
		byte eepromRead(int address, byte defaultOnError);
		byte eepromWrite(int address, byte value, byte blocking);

		void setFastSPI(byte speedySPI);

		unsigned int ltc186xRead();
		int ltc186xReadBipolar();
		unsigned int ltc186xReadAndChangeChannel(byte nextChannel);
		unsigned int ltc186xReadAndChangeChannel(byte nextChannel, byte unipolar);
		int ltc186xReadBipolarAndChangeChannel(byte nextChannel);
		int ltc186xReadBipolarAndChangeChannel(byte nextChannel, byte unipolar);		
		void ltc186xChangeChannel(byte nextChannel);
		void ltc186xChangeChannel(byte nextChannel, byte unipolar);
		void ltc186xSleep();
		void ltc186xWake();

	private:
		void internalChangeChannel(byte nextChannel, byte unipolar);
		uint8_t init_status;
		uint8_t i2cAddr_eeprom;
		uint8_t current186xConfig;
		uint8_t ltc186xDeviceType;
		int cs_pin;
		byte flags;
		char eui48[6*2+1];
};

#endif // ARD1863_H


