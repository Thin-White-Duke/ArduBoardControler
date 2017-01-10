// Config.h

/*
Copyright 2015 - 2017 Andreas Chaitidis Andreas.Chaitidis@gmail.com

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

//Config RX

#ifndef _CONFIG_h
#define _CONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define DEBUG

// Definition of Pipe
const uint64_t pipe = 0xE8E8F0F0E1LL;

//**for Mega2560
#ifdef __AVR_ATmega2560__

//Pin definition
#define CEPIN	9
#define CSPIN	10

//Definition of Serial ports
#define SERIALIO Serial1
#define DEBUGSERIAL Serial

#endif //__AVR_ATmega2560__

//**for Arduino Nano
#ifdef ARDUINO_AVR_NANO

//Pin definition

//nRF24: 

/*nRF24	>	Ardunio nano
------------------------
VCC		>	3.3 V
GND		>	GND
MOSI	>	11
MISO	>	12
SCK		>	13
IRQ		>	not connected*/

#define CEPIN	9
#define CSPIN	10

/*VESC UART
VESC		Arduino Nano
VCC		>	5V			(black wire)	
GND		>	GND			(white wire)
TX		>	RX			(orange wire)
RX		>	TX			(green wire)
*/

//Definition of Serial ports
#define SERIALIO Serial
#define DEBUGSERIAL Serial

#endif // ARDUINO_AVR_NANO

//** Teensy3.1
#ifdef TEENSYDUINO

// Serial Ports
#define SERIALIO Serial1
#define DEBUGSERIAL Serial

/* nRF24 > Teensy3.1
-----------------------
VCC      >     3.3 V
GND      >     GND
MOSI     >     11
MISO     >     12
SCK      >     13
IRQ      >     not connected*/

// nRF24 SPI definition
#define NRF_CE         9 //CEPIN
#define NRF_CS        10 //CSPIN
#define MOSI          11
#define MISO          12
#define SCK           13

/*VESC UART
VESC    Arduino Nano
VCC   > 5V      (black wire)
GND   > GND     (white wire)
TX    > RX1      (orange wire)
RX    > TX1      (green wire)
*/

#endif // TEENSYDUINO

//Define voltage control setting
#define LIPO
#ifdef  LIPO
#define MINVOLRANGE 3.3
#define MAXVOLRANGE 4.25
#define MAXNUMBERCELLS  10
#endif

// TX Voltage measurement
#define VOLTAGE_DIVISOR_TX  102.5

// WS2812 settings
#ifdef STATUS_LED_USED
#define NUM2812      4
#define BRIGHTNESS  20
#define LED_TX       0  // TX-Voltage
#define LED_TRANS    1  // Transmission
#define LED_VOLTAGE  2
#define LED_FOUR     3
#endif

// Vibrator settings
#define STRENGTH   255 // 0-255
#define PULS       150 // ms

//#define SEND_LR
#define JOYSTICKBUTTON_DEADBAND   256

// Drive train parameters
#define DIA_WHEEL                  83 // mm
#define RATIO_GEAR                3.2
#define PULSE_REV                  42 //  Number of poles*3
#define ERPM_REV                    7 //  Number of poles/2
#define CORRECT_FACTOR_DISTANCE 0.825 // required out of practical tests
#define AVERAGE_CYCLE             100

struct calcValues {
  int   numberCellsVesc = 0;
  int   numberCellsTx = 0;
  int   VescPersCap = 0;
  int   TxPersCap = 0;
  float speed = 0;
  float maxSpeed = 0;
  float distanceTravel = 0;
  float rpmAverage = 0;
  float currentAverage = 0;
  float maxCurrent = 0;
};


//Settings for Nunchuk control
#define SET_NUNCHUK_CONTROL

//Setting for current control
//#define SET_CURRENT_CONTROL

#define DEADBAND_POS 1
#define DEADBAND_NEG -1

//Settings for remote
#define TIMEOUTMAX	500

#endif // _CONFIG_h
