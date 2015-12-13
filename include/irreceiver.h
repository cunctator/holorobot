/*
 * Holorobot - a control program for a holonomic Lego robot
 * Copyright (C) 2015  Viktor Rosendahl <viktor.rosendahl@gmail.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IRRECEIVER_H
#define IRRECEIVER_H

#include <sensor.h>
#include <cstring>

#define IRRCV_MODE_SINGLE_CH	"1-MOTOR"
#define IRRCV_MODE_ALL_CH	"8-MOTOR"
#define IRRCV_MODE_ERROR	"error"
#define IRRCV_MODE_MAXLEN	sizeof(IRRCV_MODE_SINGLE_CH)

#define IRRCV_DRIVER_NAME	"ht-nxt-ir-receiver"
#define IRRCV_BIN_DATA_FORMAT	"s8"

#define IRRCV_NUM_VALUES_SINGLE (1)
#define IRRCV_NUM_VALUES_ALL	(8)
#define IRRCV_NUM_VALUES_MAX    (IRRCV_NUM_VALUES_ALL)

#define IRRCV_EXPECTED_FORMAT    (Sensor::FORMAT_S8)

class IrValues {
public:
	int8_t ch[IRRCV_NUM_VALUES_MAX];
};

class IrReceiver : public Sensor
{
public:
	enum Mode {
		MODE_SINGLE_CH = 0,
		MODE_ALL_CH,
		MODE_ERROR,
		NR_MODES
	};
	IrReceiver();
	~IrReceiver();
	bool connect(const char *portname);
	enum Mode getMode();
	__always_inline void getAllValues(IrValues *values);
	__always_inline int8_t getValue();
	__always_inline int8_t getValue(unsigned int channelAB);
	int8_t getValueA(unsigned int channel);
	int8_t getValueB(unsigned int channel);
	bool postConnect();
	bool readFromDrivers();
	bool setMode(enum Mode mode);
private:
	bool scanParams();
	int8_t channelData[IRRCV_NUM_VALUES_MAX];
	static const char modeNames[][IRRCV_MODE_MAXLEN];
	unsigned int numValues;
	static const char driverName[sizeof(IRRCV_DRIVER_NAME)];
	enum Mode mode;
};

__always_inline void IrReceiver::getAllValues(IrValues *values)
{
	memcpy(values->ch, &channelData[0], sizeof(channelData));
}

__always_inline int8_t IrReceiver::getValue()
{
	return channelData[0];
}

__always_inline int8_t IrReceiver::getValue(unsigned int channelAB)
{
	return channelData[channelAB];
}

#endif /* IRRECEIVER */
