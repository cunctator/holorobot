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

#include "project.h"
extern "C" {
#include <sys/types.h>
#include <dirent.h>
}

typedef enum {
	MOTOR_PORT_A = 0,
	MOTOR_PORT_B,
	MOTOR_PORT_C,
	MOTOR_PORT_D,
	MOTOR_NR_PORTS
} motorport_t;

/* 23 for motorRootPath, NAME_MAX for motor name and then a / and null */
#define MOTORPATH_MAX (23 + NAME_MAX + 1 + 1)

class Motor
{
public:
	Motor();
	~Motor();
	bool connect(motorport_t port);
private:
	static const char portNames[][5];
	static const char motorRootPath[];
	char motorPath[MOTORPATH_MAX];
};

#endif
