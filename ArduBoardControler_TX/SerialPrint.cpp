/*
Copyright 2016 Thin White Duke

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

#include "SerialPrint.h"

void SerialPrint(uint8_t* data, int len) {

	//	DEBUGSERIAL.print("Data to display: "); DEBUGSERIAL.println(sizeof(data));

	for (int i = 0; i <= len; i++) {
		DEBUGSERIAL.print(data[i]);
		DEBUGSERIAL.print(" ");
	}
	DEBUGSERIAL.println("");
}

void SerialPrint(const bldcMeasure& values) {
	DEBUGSERIAL.print("avgMotorCurrent: "); DEBUGSERIAL.println(values.avgMotorCurrent);
	DEBUGSERIAL.print("avgInputCurrent: "); DEBUGSERIAL.println(values.avgInputCurrent);
	DEBUGSERIAL.print("dutyCycleNow: "); DEBUGSERIAL.println(values.dutyCycleNow);
	DEBUGSERIAL.print("rpm: "); DEBUGSERIAL.println(values.rpm);
	DEBUGSERIAL.print("inputVoltage: "); DEBUGSERIAL.println(values.inpVoltage);
	DEBUGSERIAL.print("ampHours: "); DEBUGSERIAL.println(values.ampHours);
	DEBUGSERIAL.print("ampHoursCharges: "); DEBUGSERIAL.println(values.ampHoursCharged);
	DEBUGSERIAL.print("tachometer: "); DEBUGSERIAL.println(values.tachometer);
	DEBUGSERIAL.print("tachometerAbs: "); DEBUGSERIAL.println(values.tachometerAbs);
}

