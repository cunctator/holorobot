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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <err.h>

unsigned int readfile(const char *pathname, char *buffer, unsigned int size)
{
	ssize_t count;
	size_t n = 0;
	int fd = open(pathname, O_RDONLY);
	if (fd < 0) {
		warn("Could not open file %s", pathname);
		return 0;
	}

	do {
		do {
			count = read(fd, buffer, size);
			if (count >= 0 || errno != EINTR)
				break;
		} while(true);

		if (count > 0) {
			n += count;
			buffer += count;
			size -= count;
		}
		if (count <= 0 || size == 0) {
			close(fd);
			return n;
		}
	} while(true);
}

#define READFILE_BUFSIZE 32

bool readfile_uint(const char *pathname, unsigned int *v)
{
	bool retval = false;
	int fd = open(pathname, O_RDONLY);
	char buffer[READFILE_BUFSIZE];
	char *c;
	ssize_t count;
	int i;
	unsigned int value;
	unsigned int digit;

	if (fd < 0) {
		warn("Could not open file %s", pathname);
		return retval;
	}

	/* This function must be fast so we cannot afford that many checks.
	 * We just hope that the number will be in the first 32 characters 
	 * and that read will return either a full buffer or until the eof */
	do {
		count = read(fd, buffer, READFILE_BUFSIZE);
		if (count >= 0 || errno != EINTR)
			break;
	} while(true);
	close(fd);

	/* I believe that this will be faster than using fscanf() */
	for (i = 0; i < count; i++) {
		c = buffer + i;
		if (*c >= 0 && *c <= 9)
			break;
	}

	if (i < count) {
		retval = true;
		value = *c - '0';
		i++;
	}

	for (; i < count; i++) {
		c = buffer + i;
		if (*c >= 0 && *c <= 9)
			break;
		digit = *c - '0';
		value *= 10;
		value += digit;
	}

	if (retval)
		*v = value;

	return retval;
}

bool readfile_int(const char *pathname, int *v)
{
	bool retval = false;
	int fd = open(pathname, O_RDONLY);
	char buffer[READFILE_BUFSIZE];
	char *c;
	ssize_t count;
	int i;
	int value;
	int digit;
	bool negative = false;

	if (fd < 0) {
		warn("Could not open file %s", pathname);
		return retval;
	}

	/* This function must be fast so we cannot afford that many checks.
	 * We just hope that the number will be in the first 32 characters 
	 * and that read will return either a full buffer or until the eof */
	do {
		count = read(fd, buffer, READFILE_BUFSIZE);
		if (count >= 0 || errno != EINTR)
			break;
	} while(true);
	close(fd);

	/* I believe that this will be faster than using fscanf() */
	for (i = 0; i < count; i++) {
		c = buffer + i;
		if (*c == '-') {
			negative = true;
			continue;
		}
		if (*c >= 0 && *c <= 9)
			break;
	}

	if (i < count) {
		retval = true;
		value = *c - '0';
		i++;
	}

	for (; i < count; i++) {
		c = buffer + i;
		if (*c >= 0 && *c <= 9)
			break;
		digit = *c - '0';
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

bool writefile(const char *pathname, const char *buffer, unsigned int size)
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
