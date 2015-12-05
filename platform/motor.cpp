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

Motor::Motor() :
	paramsOK(false)
{
	driver_name[LEGO_NAME_SIZE] = '\0';
}

Motor::~Motor()
{
}

bool Motor::connect(enum MotorPort port)
{
	DIR *dir = opendir(motorRootPath);
	struct dirent *dent;
	unsigned long n;
	const size_t pathSize = MOTORPATHNAME_MAX;
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
		n = readfile(path, buffer, bufferSize);
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

unsigned int Motor::readMotor(const char *motorFile, char *buf,
			      unsigned int bufsize)
{
	const size_t pathSize = MOTORPATHNAME_MAX;
	char path[pathSize];

	snprintf(path, pathSize, "%s/%s", motorPath, motorFile);
	return readfile(path, buf, bufsize);
}

bool Motor::readMotorUint(const char *motorfile, unsigned int *v) {
	const size_t pathSize = MOTORPATHNAME_MAX;
	char path[pathSize];

	snprintf(path, pathSize, "%s/%s", motorPath, motorfile);
	return readfile_uint(path, v);
}

bool Motor::readMotorInt(const char *motorfile, int *v) {
	const size_t pathSize = MOTORPATHNAME_MAX;
	char path[pathSize];

	snprintf(path, pathSize, "%s/%s", motorPath, motorfile);
	return readfile_int(path, v);
}

void Motor::scanParams()
{
	unsigned int n;
	bool ok;

	paramsOK = false;

	n = readMotor("driver_name", driver_name, LEGO_NAME_SIZE);
	if (n == 0)
		return;

	ok = readMotorUint("duty_cycle", &duty_cycle);
	if (!ok)
		return;

	ok = readMotorUint("duty_cycle_sp", &duty_cycle_sp);
	if (!ok)
		return;

	ok = readMotorUint("count_per_rot", &count_per_rot);
	if (!ok)
		return;

	ok = readMotorInt("speed_sp", &speed_sp);
	if (!ok)
		return;

	ok = scanCommands();
	if (!ok)
		return;
}

bool Motor::scanCommands()
{
	bool retval = false;
	const size_t bufsize = CMDSTR_MAXLEN * CMD_NR + 2;
	char buf[bufsize];
	char *lastchr;
	int i;
	enum Command cmd;
	unsigned int n;
	char *saveptr;
	char *token;
	size_t len;
	const char *cmd_cand;

	/* Begin with setting the bitmask to zero, no commands supported, bits
	 * will be added by setCommandsSupported() below */
	commands = 0;

	n = readMotor("commands", buf, bufsize - 1);
	if (n < 2)
		return false;
	/* Make sure our string is null terminated */
	lastchr = buf + n;
	*lastchr = '\0';

	token = strtok_r(buf, " ", &saveptr);
	while (token != NULL) {
		for (i = 0; i < CMD_NR; i++) {
			cmd_cand = commandNames[i];
			len = strlen(cmd_cand);
			if (token + len > lastchr)
				continue;
			if (!strncmp(cmd_cand, token, len)) {
				cmd = (enum Command) i;
				setCommandSupported(cmd);
			}
		}
		token = strtok_r(NULL, " ", &saveptr);
	}
	/* If the motor has no supported commands it's not usable and we have
	 * most likely failed somewhere above */
	if (commands != 0)
		retval = true;
	return retval;
}

/* If these are modified, then you should also modify the motorport_t enum in
 * motor.h */
const char Motor::portNames[][PORTNAME_MAXLEN] = { PORTNAME_A,
						   PORTNAME_B,
						   PORTNAME_C,
						   PORTNAME_D };

const char Motor::motorRootPath[] = MOTOR_ROOTPATH;

const char Motor::commandNames[][CMDSTR_MAXLEN] = { CMDSTR_RUN_FOREVER,
						    CMDSTR_RUN_TO_ABS_POS,
						    CMDSTR_RUN_TO_REL_POS,
						    CMDSTR_RUN_TIMED,
						    CMDSTR_RUN_DIRECT,
						    CMDSTR_STOP,
						    CMDSTR_RESET };

const char stopCommandNames[][STOPSTR_MAXLEN] = { STOPSTR_COAST,
						  STOPSTR_BRAKE,
						  STOPSTR_HOLD };
