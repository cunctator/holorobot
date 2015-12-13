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

#include <sensor.h>
#include <fileop.h>
#include <strlib.h>

extern "C" {
#include <alloca.h>
#include <sys/types.h>
#include <dirent.h>
}

#include <cstring>
#include <cstdio>

Sensor::Sensor() :
	paramsOK(false)
{
	driver_name[LEGO_NAME_SIZE] = '\0';
}

Sensor::~Sensor()
{}

bool Sensor::connect(const char *portname, const char *drivername)
{
	DIR *dir;
	struct dirent *dent;
        unsigned int n;
	const size_t pathSize = SPATHNAME_MAX;
	char path[pathSize];
	unsigned int portlen = strlen(portname);
	unsigned int driverlen = strlen(drivername);
	unsigned int bufferSize = HMAX(portlen, driverlen) + 1;
	char *buffer;

	dir  = opendir(sensorRootPath);
	if (dir == nullptr)
		return false;
	buffer = (char*) alloca(bufferSize * sizeof(char));
	if (buffer == nullptr)
		goto failed;

	do {
		dent = readdir(dir);
		if (dent == nullptr)
			goto failed;
		/* We disregard all directories that start with a . */
		if (dent->d_name[0] == '.')
			continue;
		snprintf(path, pathSize, "%s/%s/port_name", sensorRootPath,
			 dent->d_name);
		memset(buffer, 0, bufferSize);
		n = readfile(path, buffer, bufferSize);
		/* We only care about the first portname chars, if they are
		 * contain the correct magic in* */
		if (n >= portlen && strncmp(buffer, portname, portlen) == 0) {
			snprintf(path, pathSize, "%s/%s/driver_name",
				 sensorRootPath, dent->d_name);
			memset(buffer, 0, bufferSize);
			n = readfile(path, buffer, bufferSize);
			if (n >= driverlen &&
			    strncmp(buffer, drivername, driverlen))
				break;
			else
				goto failed;
		}
	} while(true);
	snprintf(sensorPath, sizeof(sensorPath), "%s/%s", sensorRootPath,
		 dent->d_name);	
	closedir(dir);
	scanParams();
	return paramsOK && postConnect();
failed:
	closedir(dir);
	return false;
}

enum Sensor::BinFormat Sensor::getBinFormat()
{
	const unsigned int bufsize = FMTSTR_MAXLEN;
	unsigned int n;
	char buf[bufsize];
	int fmtint;

	n = readSensor("bin_data_format", buf, bufsize);
	fmtint = find_str_in_array(buf, n, formatName, FORMAT_NONE);
	return (enum BinFormat) fmtint;
}

unsigned int Sensor::getMode(char *mode, unsigned int size)
{
	return readSensor("mode", mode, size);
}

unsigned int Sensor::getModes(char *modes, unsigned int size)
{
	return readSensor("modes", modes, size);
}

bool Sensor::getNumValues(int *num)
{
	return readSensorInt("num_values", &num_values);
}

bool Sensor::setMode(const char *mode)
{
	unsigned int modesize = strlen(mode);
	return writeSensor("mode", mode, modesize);
}

unsigned int Sensor::readSensor(const char *sensorFile, char *buf,
				unsigned int bufsize)
{
	return __readSensorBin(sensorFile, buf, bufsize * sizeof(char));
}

unsigned int Sensor::readSensorBin(const char *sensorFile, void *buf,
				   unsigned int bufsize)
{
	return __readSensorBin(sensorFile, buf, bufsize);
}

bool Sensor::readSensorInt(const char *sensorFile, int *v) {
	const size_t pathSize = SPATHNAME_MAX;
	char path[pathSize];

	snprintf(path, pathSize, "%s/%s", sensorPath, sensorFile);
	return readfile_int(path, v);
}

void Sensor::scanParams()
{
	unsigned int n;
	bool ok;

	paramsOK = false;
	bzero(driver_name, sizeof(driver_name));
	bzero(port_name, sizeof(port_name));
	num_values = 0;

	bin_data_format = getBinFormat();
	if (bin_data_format == FORMAT_NONE)
		return;

	ok = readSensorInt("num_values", &num_values);
	if (!ok)
		return;

	ok = readSensorInt("poll_ms", &poll_ms);
	if (!ok)
		return;

	n = readSensor("driver_name", driver_name, LEGO_NAME_SIZE);
	if (n == 0)
		return;
	driver_name[n] = '\0'; /* Make sure we are null terminated */

	n = readSensor("port_name", port_name, LEGO_NAME_SIZE);
	if (n == 0)
		return;
	port_name[n] = '\0';	
}

bool Sensor::writeSensor(const char *sensorFile, const char *buf,
			 unsigned int size)
{
	const size_t pathSize = SPATHNAME_MAX;
	char path[pathSize];

	snprintf(path, pathSize, "%s/%s", sensorPath, sensorFile);
	return writefile(path, buf, size);
}

const char Sensor::sensorRootPath[] = SENSOR_ROOTPATH;

const char Sensor::formatName[][FMTSTR_MAXLEN] =
{
	FMTSTR_U8,
	FMTSTR_S8,
	FMTSTR_U16,
	FMTSTR_S16,
	FMTSTR_S16_BE,
	FMTSTR_U32,
	FMTSTR_S32,
	FMTSTR_FLOAT,
	FMTSTR_NONE
};
