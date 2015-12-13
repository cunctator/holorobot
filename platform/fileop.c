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

#include <fileop.h>

unsigned int readfile(const char *pathname, char *buffer, unsigned int size)
{
	return __readbinfile(pathname, buffer, size * sizeof(char));
}

unsigned int readbinfile(const char *pathname, void *buffer, unsigned int size)
{
	return __readbinfile(pathname, buffer, size);
}

bool readfile_int(const char *pathname, int *v)
{
	return __readfile_int(pathname, v);
}

bool readfile_uint(const char *pathname, unsigned int *v)
{
	return __readfile_uint(pathname, v);
}

bool writefile(const char *pathname, const char *buffer, unsigned int size)
{
	return __writefile(pathname, buffer, size);
}

bool writefile_int(const char *pathname, int value)
{
	return __writefile_int(pathname, value);
}

bool checkfile_contains(const char *pathname, const char *str)
{
	return __checkfile_contains(pathname, str);
}
