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

#ifndef FILEOP_H
#define FILEOP_H

#ifdef __cplusplus

#include <cstdbool>
extern "C" {

#else	
#include <stdbool.h>
#endif

#include <sys/types.h>

unsigned int readFile(const char *pathname, char *buffer, unsigned int size);

bool writeFile(const char *pathname, const char *buffer, unsigned int size);

#ifdef __cplusplus
}
#endif
	
#endif /* FILEOP_H */
