/*
 * ArduBoardControler.ino
 *
 * Created: 9/5/2015 7:12:40 PM
 * Author: AC
 */ 

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

#include <SPI.h>

#include "Config.h"
#include "datatypes.h"
#include "local_datatypes.h"
#include "LiPoCheck.h"

#include "nRF24L01.h" // nRF24 library from https://github.com/TMRh20/RF24
#include "RF24.h"

#ifdef OLED_USED
#include "U8glib.h"
//#include "Ucglib.h"
#endif

#ifdef STATUS_LED_USED
#include <Adafruit_NeoPixel.h>
#include "WS2812Color.h"
#endif

#ifdef DEBUG
//#include "VescUart.h" // SerialPrint for received Data Package
#include "SerialPrint.h" // SerialPrint for received Data Package
#endif

//#include "printf.h" not used, instead try
//#define printf Serial.printf

//
// Hardware configuration
//
//Set up LED WS2812
#ifdef STATUS_LED_USED
Adafruit_NeoPixel Led = Adafruit_NeoPixel(NUM2812, LED_PIN, NEO_GRB + NEO_KHZ800);
#endif

// Set up nRF24L01 radio on SPI bus plus pins NRF_CE & NRF_CS
RF24 radio(NRF_CE, NRF_CS);

// OLED display
// Check usage of the constructor for your OLED driver in the u8glib or other
#ifdef OLED_USED
//U8GLIB_SSD1306_128X64 u8g(OLED_CS, OLED_DC, MISO, MOSI, SCK); // SSD1306 with HW_SPI
U8GLIB_SSD1351_128X128_HICOLOR u8g(OLED_CS, OLED_DC, OLED_RST);
//Ucglib_SSD1351_18x128x128_HWSPI ucg(OLED_DC, OLED_CS, OLED_RST);
//Ucglib_SSD1351_18x128x128_SWSPI ucg(SCK, MOSI, OLED_DC, OLED_CS, OLED_RST);
#endif

struct remotePackage remPack;
struct bldcMeasure VescMeasuredValues;
struct calcValues calculatedValues;

// Declaration of global variables and constants
long failedCounter = 0;
boolean sendOK = false;
boolean recOK = false;
int averageCycles = 2;

enum Display {
	MIN_DISPLAY_ENUM,
	DEF_SCR,
	CURRENT_SCR,
	SPEED_SCR,
	CAP_SCR,
	DISTANCE_SCR,
	MAH_SCR,
	MAXSPEED_SCR,
	MAXCURRENT_SCR,
	MAX_DISPLAY_ENUM
};
int numberDisplayShown = DEF_SCR;

#ifndef SEND_LR
enum JoyStatus {
	center,
	left,
	right
};
JoyStatus joyStatus;
int leftright = 127;
#endif

const float ratioRpmSpeed = (DIA_WHEEL * 3.14156 * 60) / (ERPM_REV * RATIO_GEAR * 1000000);  //ERPM to Km/h
const float rationRotDist = ((DIA_WHEEL * 3.14156) / (PULSE_REV * RATIO_GEAR * 1000000))*CORRECT_FACTOR_DISTANCE; //Pulses to Km

//function declaration

void inline Vibrator();
void inline Vibrator(int numberCycles);
void BatCapIndLED(int led, float voltage, int numberCells);

#ifdef OLED_USED
void DrawScreenMain(void);
void DrawScreenSingleValue(float value, char digits[3]);
#endif

void setup() {

#ifdef DEBUG
  Serial.begin(9600);
  //Serial.println("Tx Started");
#endif

#ifdef STATUS_LED_USED
	// Led class is started and brightness is defined
  Led.begin();
	Led.setBrightness(BRIGHTNESS);
	// Some light play at startup
	Led.setPixelColor(0, COLOR_BLUE);
	Led.show();
	delay(300);
	Led.setPixelColor(1, COLOR_GREEN);
	Led.show();
	delay(300);
	Led.setPixelColor(2, COLOR_RED);
	Led.show();
	delay(300);
	Led.setPixelColor(3, COLOR_ORANGE);
	Led.show();
	delay(300);
	for (size_t i = 0; i < NUM2812; i++) {
		Led.setPixelColor(i, COLOR_OFF);
	}
	Led.show();
#endif

	// Initialization of Radio
	radio.begin();
	radio.enableAckPayload();
	radio.enableDynamicPayloads();
	radio.openWritingPipe(pipe);

	// Initialization of buttons
	pinMode(UPPER_BUTTON, INPUT_PULLUP);
	pinMode(LOWER_BUTTON, INPUT_PULLUP);

	// Initialistation of Vibrator
	pinMode(VIBRATOR_PIN, OUTPUT);

	// Number of cells for tx is calculates once
	calculatedValues.numberCellsTx = CountCells((float) (analogRead(VOLTAGE_PIN))/ VOLTAGE_DIVISOR_TX);

	// Short vibrator feedback
	Vibrator(3);

} //setup()

void loop() {

	// Determine number of cells on the board once. Must be in loop()
  // since it is not know when the board is powered on.
	if (calculatedValues.numberCellsVesc == 0) {
	  calculatedValues.numberCellsVesc = CountCells(VescMeasuredValues.inpVoltage);
	}

	// Averaging of values from the VESC
	if (averageCycles < AVERAGE_CYCLE) {
		calculatedValues.currentAverage = ((averageCycles - 1) * calculatedValues.currentAverage + VescMeasuredValues.avgMotorCurrent) / averageCycles;
		calculatedValues.rpmAverage = ((averageCycles - 1) * calculatedValues.rpmAverage + VescMeasuredValues.rpm) / averageCycles;
		averageCycles++;
	}
	else {
		calculatedValues.currentAverage = ((averageCycles - 1) * calculatedValues.currentAverage + VescMeasuredValues.avgMotorCurrent) / averageCycles;
		calculatedValues.rpmAverage = ((averageCycles - 1) * calculatedValues.rpmAverage + VescMeasuredValues.rpm) / averageCycles;
		averageCycles = 2;
	}

	calculatedValues.VescPersCap = CapCheckPerc(VescMeasuredValues.inpVoltage, calculatedValues.numberCellsVesc);
	calculatedValues.TxPersCap = CapCheckPerc(((float)analogRead(VOLTAGE_PIN) / VOLTAGE_DIVISOR_TX), calculatedValues.numberCellsTx);
	calculatedValues.speed = calculatedValues.rpmAverage * ratioRpmSpeed;
	calculatedValues.distanceTravel = VescMeasuredValues.tachometer * rationRotDist;

	// Save maximum speed and current
	calculatedValues.maxSpeed   = max(calculatedValues.speed,calculatedValues.maxSpeed);
	calculatedValues.maxCurrent = max(calculatedValues.currentAverage,calculatedValues.maxCurrent);

#ifdef STATUS_LED_USED
	BatCapIndLED(LED_TX, ((float)analogRead(VOLTAGE_PIN) / VOLTAGE_DIVISOR_TX), calculatedValues.numberCellsTx);
	BatCapIndLED(LED_VOLTAGE, VescMeasuredValues.inpVoltage, calculatedValues.numberCellsVesc);
#endif

	// Read input commands and send them to RX
#ifdef SEND_LR // XJoy is used in the RX
	remPack.valXJoy = map(analogRead(JOY_X), 0, 1023, 0, 255);
#else         //  XJoy is used in the TX
	remPack.valXJoy = 127; // send default to RX
	leftright = analogRead(JOY_X);
#endif
	remPack.valYJoy = map(analogRead(JOY_Y), 0, 1023, 0, 255);
	remPack.valLowerButton = !digitalRead(LOWER_BUTTON);
	remPack.valUpperButton = !digitalRead(UPPER_BUTTON);
	sendOK = radio.write(&remPack, sizeof(remPack));

	// Get AckPayload if an ack payload was received in the most recent call to write()
	while (radio.isAckPayloadAvailable())	{
		radio.read(&VescMeasuredValues, sizeof(VescMeasuredValues));
		recOK = true;
	}

	if (sendOK) {
#ifdef DEBUG
		Serial.print("X= ");  Serial.print  (remPack.valXJoy);
		Serial.print(" Y= "); Serial.println(remPack.valYJoy);
		Serial.println("Send successfully!");
		Serial.print("Failed= "); Serial.println(failedCounter);
#endif
#ifdef STATUS_LED_USED
		Led.setPixelColor(LED_TRANS, COLOR_GREEN);
		Led.show();
#endif
    sendOK = false;
	}
	else {
#ifdef DEBUG
	  Serial.println("Send failed!");
#endif
#ifdef STATUS_LED_USED
		Led.setPixelColor(LED_TRANS, COLOR_RED);
		Led.show();
#endif
    failedCounter++;
	}

	if (recOK) {
#ifdef DEBUG
		Serial.println("Received values from Vesc:");
		SerialPrint(VescMeasuredValues);
#endif
	}

// Use y-deflection of the joystick to switch left/right for display
#ifndef SEND_LR
  if (leftright < (512 - JOYSTICKBUTTON_DEADBAND) && joyStatus != right) {
    joyStatus = right;
    numberDisplayShown++;
    if (numberDisplayShown == MAX_DISPLAY_ENUM) {
      numberDisplayShown = DEF_SCR;
    }
    //Led.setPixelColor(LED_FOUR, COLOR_RED);
    //Led.show();
    //delay(500);
    //Led.setPixelColor(LED_FOUR, COLOR_YELLOW);
    //Led.show();
    //do something
  }
  else if (leftright > (512 + JOYSTICKBUTTON_DEADBAND) && joyStatus != left) {
    joyStatus = left;
    numberDisplayShown--;
    if (numberDisplayShown == MIN_DISPLAY_ENUM) {
      numberDisplayShown = MAX_DISPLAY_ENUM - 1;
    }
    //Led.setPixelColor(LED_FOUR, COLOR_GREEN);
    //Led.show();
    //delay(500);
    //Led.setPixelColor(LED_FOUR, COLOR_YELLOW);
    //Led.show();
  }
  else if (leftright < (512+JOYSTICKBUTTON_DEADBAND) && leftright > (512-JOYSTICKBUTTON_DEADBAND)) {
    joyStatus = center;
  }
#endif

// OLED picture loopy
#ifdef OLED_USED
  u8g.firstPage();
  do {
    switch (numberDisplayShown) {
    case DEF_SCR:
      DrawScreenMain();
      break;
    case CURRENT_SCR:
      DrawScreenSingleValue("Current",calculatedValues.currentAverage, "A");
      break;
    case SPEED_SCR:
      DrawScreenSingleValue("Speed",calculatedValues.speed, "kmh");
      break;
    case CAP_SCR:
      DrawScreenSingleValue("Capacity", calculatedValues.VescPersCap, "%");
      break;
    case MAXSPEED_SCR:
      DrawScreenSingleValue("Speed max", calculatedValues.maxSpeed, "kmh");
      break;
    case MAXCURRENT_SCR:
      DrawScreenSingleValue("Current max", calculatedValues.maxCurrent, "A");
      break;
    case DISTANCE_SCR:
      DrawScreenSingleValue("Distance", calculatedValues.distanceTravel, "km");
      break;
    case MAH_SCR:
      DrawScreenSingleValue("used mAh", (VescMeasuredValues.ampHours*1000), "mAh");
      break;
    default:
      break;
    }
  } while (u8g.nextPage());
#endif

} // loop()

void inline Vibrator() {
	analogWrite(VIBRATOR_PIN, STRENGTH);
	delay(PULS);
	analogWrite(VIBRATOR_PIN, 0);
}

void inline Vibrator(int numberCycles) {
	for (size_t i = 0; i < numberCycles; i++)	{
		Vibrator();
		delay(200);
	}
}

#ifdef STATUS_LED_USED
void BatCapIndLED(int led, float voltage, int numberCells) {
	//float capTx = CapCheckPerc(((float)analogRead(VOLTAGE_PIN) / VOLTAGE_DIVISOR_TX), calculatedValues.numberCellsTx);
	int cap = CapCheckPerc(voltage, numberCells);
	DEBUGSERIAL.print("Voltage: "); DEBUGSERIAL.println(voltage);
	DEBUGSERIAL.print("NumberCells: "); DEBUGSERIAL.println(numberCells);
	DEBUGSERIAL.print("Capacity: "); DEBUGSERIAL.println(cap);
	if (cap > 80) {
		Led.setPixelColor(led, COLOR_GREEN);
		Led.show();
		Serial.println("1");
	}
	else if (cap <= 80 && cap > 60) {
		Led.setPixelColor(led, COLOR_YELLOWGREEN);
		Led.show();
		Serial.println("2");
	}
	else if (cap <= 60 && cap > 30) {
		Led.setPixelColor(led, COLOR_ORANGE);
		Led.show();
		Serial.println("3");
	}
	else if (cap <= 30) {
		Led.setPixelColor(led, COLOR_RED);
		Led.show();
		Serial.println("4");
		//For Test Purpose:
		uint8_t offset = 0;
	}
}
#endif

#ifdef OLED_USED
void DrawScreenMain(void) {
	// graphic commands to redraw the complete screen should be placed here 
	u8g.setFontPosTop();
	u8g.setFont(u8g_font_courB08);
	u8g.setPrintPos(0, 0);
	u8g.print(calculatedValues.TxPersCap);
	u8g.drawStr(25, 0, "%");
	u8g.drawStr(50, 0, "con");
	u8g.setPrintPos(90, 0);
	u8g.print(failedCounter);
	u8g.drawStr(110, 0, "Err");
	u8g.drawHLine(0, 9, 128);
	u8g.setFont(u8g_font_courB14r);
	u8g.setFontPosTop();
	u8g.setPrintPos(0, 11);
	u8g.print(calculatedValues.speed, 1);
	u8g.setFont(u8g_font_courB08);
	u8g.setFontPosTop();
	u8g.drawStr(48, 11, "km/h");
	u8g.setFont(u8g_font_courB14r);
	u8g.setFontPosTop();
	u8g.setPrintPos(0, 30);
	u8g.print(calculatedValues.distanceTravel, 1);
	u8g.setFont(u8g_font_courB08);
	u8g.setFontPosTop();
	u8g.drawStr(48, 30, "km");
	u8g.setFont(u8g_font_courB14r);
	u8g.setFontPosTop();
	u8g.setPrintPos(78, 11);
	u8g.print(calculatedValues.currentAverage, 1);
	u8g.setFont(u8g_font_courB08);
	u8g.setFontPosTop();
	u8g.drawStr(120, 11, "A");
	u8g.drawHLine(0, 53, 128);
	u8g.setFontPosBottom();
	u8g.setFont(u8g_font_courB08);
	u8g.setPrintPos(0, 64);
	u8g.print((VescMeasuredValues.ampHours*1000), 0);
	u8g.drawStr(28, 64, "mAh");
	u8g.setPrintPos(50, 64);
	u8g.print(VescMeasuredValues.inpVoltage, 1);
	u8g.drawStr(80, 64, "V");
	u8g.setPrintPos(103, 64);
	u8g.print(calculatedValues.VescPersCap);
	u8g.drawStr(120, 64, "%");
}

void DrawScreenSingleValue(char titel[10], float value, char digits[3]) {
	u8g.setFontPosTop();
	u8g.setFont(u8g_font_courB14r);
	u8g.setPrintPos(0, 6);
	u8g.print(titel);
	u8g.setFont(u8g_font_ncenB24r);
	u8g.setFontPosTop();
	u8g.setPrintPos(0, 25);
	u8g.print(value, 1);
	u8g.setFont(u8g_font_courB14r);
	u8g.setFontPosTop();
	u8g.drawStr(80, 28, digits);
}
#endif
