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

#ifndef MOTOR_H
#define MOTOR_H

#include "lego.h"
#include "project.h"
extern "C" {
#include <sys/types.h>
#include <dirent.h>
}

#include <cstdint>

#define MOTOR_ROOTPATH "/sys/class/tacho-motor"

/* This is the maximum length of the full pathname of motor directory, it's
 * the length of motorRootPath, NAME_MAX for motor name and then a / and null */
#define MOTORPATH_MAX (sizeof(MOTOR_ROOTPATH) + NAME_MAX + 2 * sizeof(char))
/* This is the maximum legth of the full pathname of files in the motor
   directory */
#define MOTORPATHNAME_MAX \
	(sizeof(MOTOR_ROOTPATH) + 2 * NAME_MAX + 2 * sizeof(char))

#define PORTNAME_A "outA"
#define PORTNAME_B "outB"
#define PORTNAME_C "outC"
#define PORTNAME_D "outD"
#define PORTNAME_MAXLEN (sizeof(PORTNAME_A))

#define CMDSTR_RUN_FOREVER	"run-forever"
#define CMDSTR_RUN_TO_ABS_POS	"run-to-abs-pos"
#define CMDSTR_RUN_TO_REL_POS	"run-to-rel-pos"
#define CMDSTR_RUN_TIMED	"run-timed"
#define CMDSTR_RUN_DIRECT	"run-direct"
#define CMDSTR_STOP		"stop"
#define CMDSTR_RESET		"reset"
#define CMDSTR_MAXLEN (sizeof(CMDSTR_RUN_TO_ABS_POS))

#define STOPSTR_COAST		"coast"
#define STOPSTR_BRAKE		"brake"
#define STOPSTR_HOLD		"hold"
#define STOPSTR_MAXLEN (sizeof(STOPSTR_COAST))

class Motor
{
public:
	Motor();
	~Motor();
	enum MotorPort {
		MOTOR_PORT_A = 0,
		MOTOR_PORT_B,
		MOTOR_PORT_C,
		MOTOR_PORT_D,
		MOTOR_NR_PORTS
	};
	enum Command {
		CMD_RUN_FOREVER = 0,
		CMD_RUN_TO_ABS_POS,
		CMD_RUN_TO_REL_POS,
		CMD_RUN_TIMED,
		CMD_RUN_DIRECT,
		CMD_STOP,
		CMD_RESET
	};
	enum StopCommand {
		STOPCMD_COAST = 0,
		STOPCMD_BRAKE,
		STOPCMD_HOLD
	};
	bool connect(enum MotorPort port);
private:
	void scanParams();
	static const char portNames[][5];
	static const char motorRootPath[];
	static const char commandNames[][CMDSTR_MAXLEN];
	static const char stopCommandNames[][STOPSTR_MAXLEN];
	char motorPath[MOTORPATH_MAX];
	/* These are meant to be various parameters that tells us about
	 * the motor, these are the ones that make sense to cache in the
	 * Motor objects, other we read every time from sysfs because they
	 * are volatile and don't make sense to cache in the object */
	enum MotorPort port;
	enum Command command;
	uint32_t commands;
	unsigned int count_per_rot;
	char driver_name[LEGO_NAME_SIZE + 1];
	bool speed_regulation;
	unsigned int duty_cycle;
	unsigned int duty_cycle_sp;
	unsigned int speed_sp;
	uint32_t supported_stop_commands;
	enum StopCommand stop_command;
	/* This one tells if the params make sense, e.g. is the result of
	 * sucessfully reading the from a /syc/class/tacho-motor/mororX/
	 * directory
	 */
	bool paramsOK;
};

#endif
