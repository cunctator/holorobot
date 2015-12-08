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

#ifndef SENSOR_H
#define SENSOR_H

#include <lego.h>
#include <project.h>
#include <fileop.h>

extern "C" {
#include <sys/types.h>
#include <dirent.h>
}

#include <cstdint>

#define SENSOR_ROOTPATH "/sys/class/lego-sensor"

/* This is the maximum legth of the full pathname of files in the sensor
 * directory */
#define SPATHNAME_MAX \
	(sizeof(SENSOR_ROOTPATH) + 2 * NAME_MAX + 2 * sizeof(char))

#define PORTNAME_MAX 64

#define FMTSTR_U8	"u8"
#define FMTSTR_S8	"s8"
#define FMTSTR_U16	"u16"
#define FMTSTR_S16	"s16"
#define FMTSTR_S16_BE	"s16_be"
#define FMTSTR_U32	"u32"
#define FMTSTR_S32	"s32"
#define FMTSTR_FLOAT	"float"
#define FMTSTR_NONE	"none"
#define FMTSTR_MAXLEN	(sizeof(FMTSTR_S16_BE))

class Sensor
{
public:
	Sensor();
	virtual ~Sensor();
	int getPoll_ms();
	bool setPoll_ms(int value);
protected:
	enum BinFormat {
		FORMAT_U8 = 0,
		FORMAT_S8,
		FORMAT_U16,
		FORMAT_S16,
		FORMAT_S16_BE,
		FORMAT_U32,
		FORMAT_S32,
		FORMAT_FLOAT,
		FORMAT_NONE,
		FORMATS_NR
	};
	bool connect(char *portname, char *drivername);
	virtual bool postConnect();
	enum BinFormat getBinFormat();
	bool getMode(char *mode, unsigned int modelen);
	unsigned int readSensor(const char *sensorFile, char *buf,
				unsigned int bufsize);
	__always_inline unsigned int __readSensorBin(const char
						     *sensorFile,
						     void *buf,
						     unsigned int bufsize);
	unsigned int readSensorBin(const char *sensorFile, void *buf,
				   unsigned int bufsize);
	bool readSensorInt(const char *sensorFile, int *v);
	void scanParams();
	bool setMode(char *mode);
	/* This one tells if the params make sense, e.g. is the result of
	 * sucessfully reading the from a /syc/class/lego-sensor/sensorX/
	 * directory
	 */
	bool paramsOK;
private:
	char binDataPath[SPATHNAME_MAX];
	const static char sensorRootPath[];
	enum BinFormat bin_data_format;
	char driver_name[LEGO_NAME_SIZE + 1];
	int num_values;
	int poll_ms;
	const static char formatName[][FMTSTR_MAXLEN];
	char port_name[LEGO_NAME_SIZE + 1];
	char sensorPath[SPATHNAME_MAX];
};

__always_inline unsigned int Sensor::__readSensorBin(const char
						     *sensorFile,
						     void *buf,
						     unsigned int bufsize)
{
	const size_t pathSize = SPATHNAME_MAX;
	char path[pathSize];

	snprintf(path, pathSize, "%s/%s", sensorPath, sensorFile);
	return readbinfile(path, buf, bufsize);
}

#endif /* SENSOR_H */
