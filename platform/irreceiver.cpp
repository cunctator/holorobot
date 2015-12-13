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

#include <irreceiver.h>
#include <strlib.h>

IrReceiver::IrReceiver() :
	Sensor()
{
}

IrReceiver::~IrReceiver()
{
}

bool IrReceiver::connect(const char *portname)
{
	return Sensor::connect(portname, driverName);
}

bool IrReceiver::postConnect()
{
	return scanParams();
}

bool IrReceiver::readFromDrivers()
{
	unsigned int n;
	n = __readSensorBin("bin_data", channelData, sizeof(channelData));

	if (n / sizeof(int8_t) < numValues)
		return false;
	return true;
}

bool IrReceiver::setMode(IrReceiver::Mode mode)
{
	bool rval;
	IrReceiver::Mode newMode;

	rval = Sensor::setMode(modeNames[mode]);	
	if (!rval)
		return rval;

	newMode = getMode();
	if (newMode != mode)
		return false;

	return scanParams();
}

enum IrReceiver::Mode IrReceiver::getMode()
{
	unsigned int n;
	char modestr[LEGO_NAME_SIZE + 1];
	int modeint;

	n = Sensor::getMode(&modestr[0], LEGO_NAME_SIZE);
	if (n == 0)
		return IrReceiver::MODE_ERROR;

	modeint = find_str_in_array(modestr, n, modeNames, MODE_ERROR);
	return (IrReceiver::Mode) modeint;
}

bool IrReceiver::scanParams()
{
	bool ok;
	int nvalues;
	enum Mode mode;
	enum Sensor::BinFormat fmt;

	mode = IrReceiver::getMode();
	if (mode == MODE_ERROR)
		return false;

	ok = Sensor::getNumValues(&nvalues);
	if (!ok)
		return false;

	switch (mode) {
	case MODE_SINGLE_CH:
		if (nvalues != IRRCV_NUM_VALUES_SINGLE)
			return false;
		break;
	case MODE_ALL_CH:
		if (nvalues != IRRCV_NUM_VALUES_ALL)
			return false;
		break;
	default:
		return false;
	};

	numValues = (unsigned int) nvalues;

	fmt = Sensor::getBinFormat();
	if (fmt != IRRCV_EXPECTED_FORMAT)
		return false;
	
	ok = readFromDrivers();
	if (!ok)
		return false;

	return true;
}

const char IrReceiver::driverName[] = IRRCV_DRIVER_NAME;

const char IrReceiver::modeNames[][IRRCV_MODE_MAXLEN] = {
	IRRCV_MODE_SINGLE_CH,
	IRRCV_MODE_ALL_CH,
	IRRCV_MODE_ERROR
};
