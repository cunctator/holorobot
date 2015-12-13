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

#ifndef STRLIB_H
#define STRLIB_H

#ifdef __cplusplus
#include <cstring>
extern "C" {
#else
#include <string.h>
#endif


#define __find_str_in_array(STR, LEN, ARRAY, ARRAY_LEN) \
	(__find_str_in_matrix(STR, LEN, &(ARRAY[0][0]), ARRAY_LEN, \
			   sizeof(ARRAY[0]) / sizeof(char)))

#define find_str_in_array(STR, LEN, ARRAY, ARRAY_LEN) \
	(find_str_in_matrix(STR, LEN, &(ARRAY[0][0]), ARRAY_LEN, \
			    sizeof(ARRAY[0]) / sizeof(char)))
	
int find_str_in_matrix(char *str,
		       unsigned int len,
		       const char *array,
		       const unsigned int array_len,
		       const unsigned int array_strlen);	

__always_inline int __find_str_in_matrix(char *str,
					 unsigned int len,
					 const char *array,
					 const unsigned int array_len,
					 const unsigned int array_strlen)
{
	int i;
	unsigned int an;
	const char *astr = array;

	for (i = 0; i < (int) array_len; i++) {
		an = strlen(astr);
		if (an > len)
			continue;
		if (!strncmp(str, astr, an))
			break;
		astr += array_strlen;
	}
	return i;
}

#ifdef __cplusplus
}
#endif
	
#endif /* STRLIB_H */
