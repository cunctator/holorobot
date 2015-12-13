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
#include <cstdint>
#include <cstring>
extern "C" {

#else	
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#endif

#include <alloca.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <err.h>

static __always_inline
unsigned int __readbinfile(const char *pathname, void *buffer,
			   unsigned int size)
{
	uint8_t *buf = (uint8_t*) buffer;
	ssize_t count;
	size_t n = 0;
	int fd = open(pathname, O_RDONLY);
	if (fd < 0) {
		warn("Could not open file %s", pathname);
		return 0;
	}

	do {
		do {
			count = read(fd, buf, size);
			if (count >= 0 || errno != EINTR)
				break;
		} while(true);

		if (count > 0) {
			n += count;
			buf += count;
			size -= count;
		}
		if (count <= 0 || size == 0) {
			close(fd);
			return n;
		}
	} while(true);
}

#define READFILE_BUFSIZE 32

static __always_inline
bool __readfile_uint(const char *pathname, unsigned int *v)
{
	bool retval = false;
	char buffer[READFILE_BUFSIZE];
	char c;
	unsigned int count;
	unsigned int i;
	unsigned int value;
	unsigned int digit;

	count = __readbinfile(pathname, buffer, sizeof(buffer));

	/* I believe that this will be faster than using fscanf() */
	for (i = 0; i < count; i++) {
		c = buffer[i];
		if (c >= '0' && c <= '9')
			break;
	}

	if (i < count) {
		retval = true;
		value = c - '0';
		i++;
	}

	for (; i < count; i++) {
		c = buffer[i];
		if (c < '0' || c <= '9')
			break;
		digit = c - '0';
		value *= 10;
		value += digit;
	}

	if (retval)
		*v = value;

	return retval;
}

static __always_inline
bool __readfile_int(const char *pathname, int *v)
{
	bool retval = false;
	char buffer[READFILE_BUFSIZE];
	char c;
	unsigned int count;
	unsigned int i;
	int value;
	int digit;
	bool negative = false;

	count = __readbinfile(pathname, buffer, sizeof(buffer));

	/* I believe that this will be faster than using fscanf() */
	for (i = 0; i < count; i++) {
		c = buffer[i];
		if (c == '-') {
			negative = true;
			continue;
		}
		if (c >= '0' && c <= '9')
			break;
	}

	if (i < count) {
		retval = true;
		value = c - '0';
		i++;
	}

	for (; i < count; i++) {
		c = buffer[i];
		if (c < '0' || c > '9')
			break;
		digit = c - '0';
		value *= 10;
		value += digit;
	}

	if (retval) {
		if (negative)
			value = - value;
		*v = value;
	}

	return retval;
}

static __always_inline
bool __writefile(const char *pathname, const char *buffer, unsigned int size)
{
	bool retval = false;
	ssize_t count;
	int fd = open(pathname, O_WRONLY);
	if (fd < 0) {
		warn("Could not open file %s", pathname);
		return retval;
	}

	do {
		do {
			count = write(fd, buffer, size);
			if (count >= 0 || errno != EINTR)
				break;
		} while(true);

		if (count > 0) {
			buffer += count;
			size -= count;
		}
		if (size == 0 && count >= 0) {
			retval = true;
			break;
		}
	} while(count > 0);
	close(fd);
	return retval;
}

static __always_inline
bool __writefile_int(const char *pathname, int value)
{
	char buf[READFILE_BUFSIZE];
	int n;

	n = snprintf(buf, READFILE_BUFSIZE, "%d", value);
	return __writefile(pathname, buf, n);
}

static __always_inline
bool __checkfile_contains(const char *pathname, const char *str)
{
	unsigned int n = strlen(str);
	unsigned int bufsize = n * sizeof(char);
	char *buf = (char*) alloca(bufsize);
	unsigned int nr;

	nr = __readbinfile(pathname, buf, bufsize);
	if (nr < n)
		return false;
	return strncmp(buf, str, n) == 0;
}

unsigned int readfile(const char *pathname, char *buffer, unsigned int size);

unsigned int readbinfile(const char *pathname, void *buffer, unsigned int size);

bool readfile_int(const char *pathname, int *v);

bool readfile_uint(const char *pathname, unsigned int *v);

bool writefile(const char *pathname, const char *buffer, unsigned int size);

bool writefile_int(const char *pathname, int value);

bool checkfile_contains(const char *pathname, const char *str);

#ifdef __cplusplus
}
#endif
	
#endif /* FILEOP_H */
