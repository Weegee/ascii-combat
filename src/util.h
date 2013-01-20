/* util.h: Declarations for util.c.
 * Copyright (C) 2011, 2012 Weegee
 *
 * This file is part of ASCII Combat.
 *
 * ASCII Combat is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ASCII Combat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ASCII Combat.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>

enum loglevel
{
  LOG_NONE, LOG_INFO, LOG_VERBOSE, LOG_DEBUG, LOG_LEVEL = LOG_DEBUG
};

extern FILE * g_log;

void _free(void * ptr);
int get_intlen(int n);
int get_randint(int min, int max);
void write_log(int level, const char * str, ...);

#endif
