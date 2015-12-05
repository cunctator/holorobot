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

#define SPEED_REGULATION_TRUE	"on"
#define SPEED_REGULATION_FALSE  "off"
#define SPEED_REGULATION_MAXLEN (sizeof(SPEED_REGULATION_FALSE))

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
		CMD_RESET,
		CMD_NR
	};
	enum StopCommand {
		STOPCMD_COAST = 0,
		STOPCMD_BRAKE,
		STOPCMD_HOLD,
		STOPCMD_NR
	};
	bool connect(enum MotorPort port);
private:
	unsigned int readMotor(const char *motorFile, char *buf,
			       unsigned int bufize);
	bool readMotorUint(const char *motorFile, unsigned int *v);
	bool readMotorInt(const char *motorFile, int *v);
	bool writeMotor(const char *motorFile, const char *buf,
			unsigned int bufsize);
	bool writeMotorInt(const char *motorFile, int v);
	void scanParams();
	bool scanCommands();
	bool scanStopCommands();
	__always_inline void setCommandSupported(enum Command cmd);
	__always_inline void setStopCommandSupported(enum StopCommand cmd);
	__always_inline bool isCommandSupported(enum Command cmd);
	__always_inline bool isStopCommandSupported(enum StopCommand cmd);
	bool getCommandFromDriver(enum Command *cmd);
	bool getStopCommandFromDriver(enum StopCommand *cmd);
	bool getSpeedRegulationFromDriver(bool *value);
	__always_inline enum Command getCommand();
	__always_inline enum StopCommand getStopCommand();
	__always_inline bool getSpeedRegulation();
	bool setCommand(enum Command cmd);
	bool setStopCommand(enum StopCommand cmd);
	bool setDutyCycleSP(int value);
	bool setSpeedSP(int value);
	__always_inline bool getSpeed(int *value);
	static const char portNames[][5];
	static const char motorRootPath[];
	static const char commandNames[][CMDSTR_MAXLEN];
	static const char stopCommandNames[][STOPSTR_MAXLEN];
	char motorPath[MOTORPATHNAME_MAX];
	/* These are meant to be various parameters that tells us about
	 * the motor, these are the ones that make sense to cache in the
	 * Motor objects, other we read every time from sysfs because they
	 * are volatile and don't make sense to cache in the object */
	enum MotorPort port;
	enum Command command;
	/* Bitmask of supported commands */
	uint32_t commands;
	int count_per_rot;
	char driver_name[LEGO_NAME_SIZE + 1];
	bool speed_regulation;
	int duty_cycle_sp;
	int speed_sp;
	/* Bitmask of supported commands */
	uint32_t stop_commands;
	enum StopCommand stop_command;
	/* This one tells if the params make sense, e.g. is the result of
	 * sucessfully reading the from a /syc/class/tacho-motor/mororX/
	 * directory
	 */
	bool paramsOK;
};

__always_inline void Motor::setCommandSupported(enum Command cmd)
{
	commands |= 0x1 << cmd;
}

__always_inline void Motor::setStopCommandSupported(enum StopCommand cmd)
{
	stop_commands |= 0x1 << cmd;
}

__always_inline enum Motor::Command Motor::getCommand()
{
	return command;
}

__always_inline enum Motor::StopCommand Motor::getStopCommand()
{
	return stop_command;
}

__always_inline bool Motor::getSpeedRegulation()
{
	return speed_regulation;
}

__always_inline bool Motor::isCommandSupported(enum Command cmd)
{
	return (commands & (0x1 << cmd)) != 0;
}

__always_inline bool Motor::isStopCommandSupported(enum StopCommand cmd)
{
	return (stop_commands & (0x1 << cmd)) != 0;
}

__always_inline bool Motor::getSpeed(int *value)
{
	return readMotorInt("speed", value);
}

#endif
