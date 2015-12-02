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

#include <motor.h>
#include <fileop.h>

extern "C" {
#include <sys/types.h>
#include <dirent.h>
}

#include <cstring>
#include <cstdio>

Motor::Motor()
{
}

Motor::~Motor()
{
}

bool Motor::connect(motorport_t port)
{
	DIR *dir = opendir(motorRootPath);
	struct dirent *dent;
	unsigned long n;
	const size_t pathSize = MOTORPATH_MAX + 10;
	char path[pathSize];
	const size_t bufferSize = 5;
	char buffer[bufferSize];

	if (dir == nullptr)
		return false;

	do {
		dent = readdir(dir);
		if (dent == nullptr)
			goto error;
		n = snprintf(path, pathSize, "%s/%s/port_name", motorRootPath,
			     dent->d_name);
		if (n >= pathSize)
			goto error;
		memset(buffer, 0, bufferSize);
		n = readFile(path, buffer, bufferSize);
		/* We only care about the first 4 chars, if they are contain
		 * the correct magic out[ABCD] */
		if (n >= 4 && strncmp(buffer, portNames[port], 4) == 0)
			goto success;
	} while(dent != nullptr);
error:
	closedir(dir);
	return false;
success:
	snprintf(motorPath, sizeof(motorPath), "%s/%s", motorRootPath,
		 dent->d_name);
	closedir(dir);
	return true;
}

/* If these are modified, then you should also modify the motorport_t enum in
 * motor.h */
const char Motor::portNames[][5] = { "outA", "outB", "outC", "outD" };
const char Motor::motorRootPath[] = "/sys/class/tacho-motor";
