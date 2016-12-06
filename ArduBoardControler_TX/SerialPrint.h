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

#ifndef _SERIALPRINT_h
#define _SERIALPRINT_h

#include "Config.h"
#include "local_datatypes.h"

///Help Function to print struct bldcMeasure over Serial for Debug
///Define in a Config.h the DEBUGSERIAL you want to use

void SerialPrint(const struct bldcMeasure& values);

///Help Function to print uint8_t array over Serial for Debug
///Define in a Config.h the DEBUGSERIAL you want to use

void SerialPrint(uint8_t* data, int len);

#endif //_SERIALPRINT_h

