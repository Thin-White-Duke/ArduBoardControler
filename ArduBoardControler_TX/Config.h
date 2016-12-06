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

// Config TX

#ifndef _CONFIG_h
#define _CONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Additional hardware definition
#define OLED_USED         //comment if not used
//#define STATUS_LED_USED   //comment if not used

// General settings
#define DEBUG //use if debug information over serial required

// Definition of Pipe for nRF24
const uint64_t pipe = 0xE8E8F0F0E1LL;

// Pin definition

//** Arduino ATmega328p (Nano)
#ifdef ARDUINO_AVR_NANO

// Serial Ports
#define SERIALIO Serial
#define DEBUGSERIAL Serial

/* nRF24 > Ardunio nano
-----------------------
VCC      >     3.3 V
GND      >     GND
MOSI     >     11
MISO     >     12
SCK      >     13
IRQ      >     not connected*/

// nRF24 SPI definition
#define CEPIN          9
#define CSPIN			    10
#define MOSI				  11
#define MISO				  12
#define SCK					  13

// Joystick and buttons
#define JOY_X				  A0
#define JOY_Y				  A1
#define UPPER_BUTTON	 2
#define LOWER_BUTTON	 3

//other Pins
#define LED_PIN			   4
#define VOLTAGE_PIN   A2
#define VIBRATOR_PIN  A3

// OLED
// Check for correct pin setting by the used OLED driver in the u8glib
// Here I use a SSD1306 with SW_SPI
#ifdef OLED_USED
//VCC		>					5V
//GND		>				 GND
#define OLED_SCK		5	// (to RES)
#define OLED_MOSI   6	// (to DC)
#define OLED_MISO	 20	// not used
#define OLED_CEPIN	7	// (to SDA)
#define OLED_CSPIN	8	// (to SCL)
#endif //OLED_USED

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

// Joystick and buttons
#define JOY_X         A0
#define JOY_Y         A1
#define UPPER_BUTTON   2
#define LOWER_BUTTON   3

//other Pins
#define LED_PIN        4
#define VOLTAGE_PIN   A2
#define VIBRATOR_PIN  A3

// OLED
// Check for correct pin setting by the used OLED driver in the ucglib
#ifdef OLED_USED
//VCC   >       5V
//GND   >      GND
//Ucglib_SSD1351_18x128x128_HWSPI ucg(cd, cs, [reset])
//Ucglib_SSD1351_18x128x128_SWSPI ucg(sclk, data, cd, cs, [reset])
#define OLED_RST       5
#define OLED_SDCS      6
#define OLED_DC        7
#define OLED_CS        8
#define MOSI          11
#define MISO          12
#define SCK           13
#endif //OLED_USED

#endif // TEENSYDUINO

//Define voltage control setting
#define LIPO
#ifdef  LIPO
#define MINVOLRANGE 3.3
#define MAXVOLRANGE 4.25
#define MAXNUMBERCELLS	10
#endif

// TX Voltage measurement
#define VOLTAGE_DIVISOR_TX	102.5

// WS2812 settings
#ifdef STATUS_LED_USED
#define NUM2812		   4
#define BRIGHTNESS	20
#define LED_TX		   0	// TX-Voltage
#define LED_TRANS	   1	// Transmission
#define LED_VOLTAGE	 2
#define LED_FOUR	   3
#endif

// Vibrator settings
#define STRENGTH	 255 // 0-255
#define PULS       150 // ms

//#define SEND_LR
#define JOYSTICKBUTTON_DEADBAND		256

// Drive train parameters
#define DIA_WHEEL	                 83 // mm
#define RATIO_GEAR                3.2
#define PULSE_REV                  42 //  Number of poles*3
#define ERPM_REV                    7 //  Number of poles/2
#define CORRECT_FACTOR_DISTANCE 0.825 // required out of practical tests
#define AVERAGE_CYCLE             100

struct calcValues {
	int		numberCellsVesc = 0;
	int		numberCellsTx = 0;
	int		VescPersCap = 0;
	int		TxPersCap = 0;
	float	speed = 0;
	float	maxSpeed = 0;
	float	distanceTravel = 0;
	float	rpmAverage = 0;
	float	currentAverage = 0;
	float	maxCurrent = 0;
};

#endif // _CONFIG_h
