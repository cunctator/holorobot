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
			goto failed;
		/* We disregard all directories that start with a . */
		if (dent->d_name[0] == '.')
			continue;
		snprintf(path, pathSize, "%s/%s/port_name", motorRootPath,
			 dent->d_name);
		memset(buffer, 0, bufferSize);
		n = readfile(path, buffer, bufferSize);
		/* We only care about the first 4 chars, if they are contain
		 * the correct magic out[ABCD] */
		if (n >= 4 && strncmp(buffer, portNames[port], 4) == 0)
			break;
	} while(true);
	snprintf(motorPath, sizeof(motorPath), "%s/%s", motorRootPath,
		 dent->d_name);
	closedir(dir);
	return true;
failed:
	closedir(dir);
	return false;
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

bool Motor::writeMotor(const char *motorFile,
		       const char *buf,
		       unsigned int bufsize)
{
	const size_t pathSize = MOTORPATHNAME_MAX;
	char path[pathSize];

	snprintf(path, pathSize, "%s/%s", motorPath, motorFile);
	return writefile(path, buf, bufsize);
}

bool Motor::writeMotorInt(const char *motorFile, int v)
{
	const size_t pathSize = MOTORPATHNAME_MAX;
	char path[pathSize];
	const size_t strsize = 30;
	char intstr[strsize];
	unsigned int n;

	snprintf(path, pathSize, "%s/%s", motorPath, motorFile);
	n = snprintf(intstr, strsize, "%d", v);
	return writefile(path, intstr, n);
}

void Motor::scanParams()
{
	unsigned int n;
	bool ok;

	paramsOK = false;

	n = readMotor("driver_name", driver_name, LEGO_NAME_SIZE);
	driver_name[n] = '\0';
	if (n == 0)
		return;

	ok = readMotorInt("duty_cycle_sp", &duty_cycle_sp);
	if (!ok)
		return;

	ok = readMotorInt("count_per_rot", &count_per_rot);
	if (!ok)
		return;

	ok = readMotorInt("ramp_down_sp", &ramp_down_sp);
	if (!ok)
		return;

	ok = readMotorInt("ramp_up_sp", &ramp_up_sp);
	if (!ok)
		return;

	ok = readMotorInt("speed_sp", &speed_sp);
	if (!ok)
		return;

	ok = readMotorInt("time_sp", &time_sp);
	if (!ok)
		return;

	ok = scanCommands();
	if (!ok)
		return;

	ok = scanStopCommands();
	if (!ok)
		return;

	ok = getCommandFromDriver(&command);
	if (!ok)
		return;

	ok = getStopCommandFromDriver(&stop_command);
	if (!ok)
		return;

	ok = getSpeedRegulationFromDriver(&speed_regulation);
	if (!ok)
		return;
}

bool Motor::scanCommands()
{
	bool retval = false;
	const size_t bufsize = CMDSTR_MAXLEN * CMDS_NR + 2;
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
		for (i = 0; i < CMDS_NR; i++) {
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

bool Motor::scanStopCommands()
{
	bool retval = false;
	const size_t bufsize = STOPSTR_MAXLEN * STOPCMDS_NR + 2;
	char buf[bufsize];
	char *lastchr;
	int i;
	enum StopCommand cmd;
	unsigned int n;
	char *saveptr;
	char *token;
	size_t len;
	const char *cmd_cand;

	/* Begin with setting the bitmask to zero, no commands supported, bits
	 * will be added by setStopCommandsSupported() below */
	stop_commands = 0;

	n = readMotor("stop_commands", buf, bufsize - 1);
	if (n < 2)
		return false;
	/* Make sure our string is null terminated */
	lastchr = buf + n;
	*lastchr = '\0';

	token = strtok_r(buf, " ", &saveptr);
	while (token != NULL) {
		for (i = 0; i < CMDS_NR; i++) {
			cmd_cand = stopCommandNames[i];
			len = strlen(cmd_cand);
			if (token + len > lastchr)
				continue;
			if (!strncmp(cmd_cand, token, len)) {
				cmd = (enum StopCommand) i;
				setStopCommandSupported(cmd);
			}
		}
		token = strtok_r(NULL, " ", &saveptr);
	}
	/* If the motor has no supported stop commands it's fishy and we have
	 * most likely failed somewhere above */
	if (stop_commands != 0)
		retval = true;
	return retval;
}

bool Motor::getCommandFromDriver(enum Command *cmd)
{
	bool retval = false;
	const size_t bufsize = CMDSTR_MAXLEN + 2;
	char buf[bufsize];
	unsigned int n;
	int i;
	const char *cmd_cand;
	size_t len;

	n = readMotor("command", buf, bufsize - 1);
	if (n < 1)
		return retval;

	buf[n] = '\0';
	for (i = 0; i < CMDS_NR; i++) {
		cmd_cand = commandNames[i];
		len = strlen(cmd_cand);
		if (len > n)
			continue;
		/* We can do this, because if you look at the commands in
		 * motor.h, you see that while commands have common substrings,
		 * no command is a substring of another command. If commands
		 * are added so that this is no longer true, this would have
		 * to be modified to check for the longest matching string or
		 * something */
		if (!strncmp(cmd_cand, buf, len)) {
			retval = true;
			*cmd = (enum Command) i;
			break;
		}
	}
	return retval;
}

bool Motor::getStopCommandFromDriver(enum StopCommand *cmd)
{
	bool retval = false;
	const size_t bufsize = STOPSTR_MAXLEN + 2;
	char buf[bufsize];
	unsigned int n;
	int i;
	const char *cmd_cand;
	size_t len;

	n = readMotor("stop_command", buf, bufsize - 1);
	if (n < 1)
		return retval;

	buf[n] = '\0';
	for (i = 0; i < CMDS_NR; i++) {
		cmd_cand = stopCommandNames[i];
		len = strlen(cmd_cand);
		if (len > n)
			continue;
		/* We can do this, because if you look at the stop commands in
		 * motor.h, you see that no command is a substring of another
		 * command. If stop commands are added so that this is no
		 * longer true, this would have to be modified to check for the
		 * longest matching string or something */
		if (!strncmp(cmd_cand, buf, len)) {
			retval = true;
			*cmd = (enum StopCommand) i;
			break;
		}
	}
	return retval;
}

bool Motor::getSpeedRegulationFromDriver(bool *value)
{
	const size_t bufsize = SPEED_REGULATION_MAXLEN + 1;
	char buf[bufsize];
	unsigned int n;
	size_t len;

	n = readMotor("speed_regulation", buf, bufsize - 1);
	if (n < 1)
		return false;

	len = strlen(SPEED_REGULATION_TRUE);
	if (n >= len && !strncmp(buf, SPEED_REGULATION_TRUE, len)) {
		*value = true;
		return true;
	}
	len = strlen(SPEED_REGULATION_FALSE);
	if (n >= len && !strncmp(buf, SPEED_REGULATION_FALSE, len)) {
		*value = false;
		return true;
	}
	return false;
}

void Motor::readState()
{
	const size_t bufsize = STATESTR_MAXLEN * STATES_NR + 2;
	char buf[bufsize];
	char *lastchr;
	int i;
	enum State s;
	unsigned int n;
	char *saveptr;
	char *token;
	size_t len;
	const char *state_cand;

	/* Begin with setting the bitmask to zero, no commands supported, bits
	 * will be added by setCommandsSupported() below */
	state = 0;

	n = readMotor("state", buf, bufsize - 1);
	if (n < 2)
		return;
	/* Make sure our string is null terminated */
	lastchr = buf + n;
	*lastchr = '\0';

	token = strtok_r(buf, " ", &saveptr);
	while (token != NULL) {
		for (i = 0; i < CMDS_NR; i++) {
			state_cand = stateNames[i];
			len = strlen(state_cand);
			if (token + len > lastchr)
				continue;
			if (!strncmp(state_cand, token, len)) {
				s = (enum State) i;
				setStateActive(s);
			}
		}
		token = strtok_r(NULL, " ", &saveptr);
	}
}

bool Motor::setCommand(enum Command cmd)
{
	size_t len;
	bool retval = false;
	const char *cmdstr;

	if (!isCommandSupported(cmd))
		return retval;
	cmdstr = commandNames[cmd];
	len = strlen(cmdstr);
	retval = writeMotor("command", cmdstr, len);
	if (!retval)
		return retval;
	command = cmd;
	return retval;
}

bool Motor::setStopCommand(enum StopCommand cmd)
{
	size_t len;
	bool retval = false;
	const char *cmdstr;

	if (!isStopCommandSupported(cmd))
		return retval;
	cmdstr = stopCommandNames[cmd];
	len = strlen(cmdstr);
	retval = writeMotor("stop_command", cmdstr, len);
	if (!retval)
		return retval;
	stop_command = cmd;
	return retval;
}

bool Motor::setDutyCycleSP(int value)
{
	bool retval;
	retval = writeMotorInt("duty_cycle_sp", value);
	if (!retval)
		return retval;
	duty_cycle_sp = value;
	return retval;
}

bool Motor::setPosition(int value)
{
	return writeMotorInt("position", value);
}

bool Motor::setPositionSP(int value)
{
	bool retval;
	retval = writeMotorInt("position_sp", value);
	if (!retval)
		return retval;
	position_sp = value;
	return retval;
}

bool Motor::setRampDownSP(int value)
{
	bool retval;
	retval = writeMotorInt("ramp_down_sp", value);
	if (!retval)
		return retval;
	ramp_down_sp = value;
	return retval;
}

bool Motor::setRampUpSP(int value)
{
	bool retval;
	retval = writeMotorInt("ramp_up_sp", value);
	if (!retval)
		return retval;
	ramp_up_sp = value;
	return retval;
}

bool Motor::setSpeedSP(int value)
{
	bool retval;
	retval = writeMotorInt("speed_sp", value);
	if (!retval)
		return retval;
	speed_sp = value;
	return retval;
}

bool Motor::setTimeSP(int value)
{
	bool retval;
	retval = writeMotorInt("time_sp", value);
	if (!retval)
		return retval;
	time_sp = value;
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

const char Motor::stopCommandNames[][STOPSTR_MAXLEN] = { STOPSTR_COAST,
							 STOPSTR_BRAKE,
							 STOPSTR_HOLD };

const char Motor::stateNames[][STATESTR_MAXLEN] = { STATESTR_RUNNING,
						    STATESTR_RAMPING,
						    STATESTR_HOLDING,
						    STATESTR_STALLED,
						    STATESTR_NONE };
