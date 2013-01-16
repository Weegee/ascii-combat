/* util.c: Helper functions for several small tasks
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

#include "util.h"

FILE * g_log;

// Frees allocated space, removes dangling pointer
void
_free(void * ptr)
{
  free(ptr);
  ptr = NULL;
}

// Returns the length of an integer
int
get_intlen(int n)
{
  return (n == 0) ? 1 : (int) (log10((n < 0) ? abs(n) * 10 : n) + 1);
}

// Returns a random integer
int
get_randint(int min, int max)
{
  return min + rand() % (max - min + 1);
}

// Prints debug messages to the log file
void
write_log(int level, const char * str, ...)
{
  if (level <= LOG_LEVEL)
  {
    const char * prefix;
    va_list args;

    switch (level)
    {
      case LOG_INFO:
        prefix = "[I] ";
        break;
      case LOG_VERBOSE:
        prefix = "[V] ";
        break;
      case LOG_DEBUG:
        prefix = "[D] ";
        break;
      default:
        prefix = "[?] ";
        break;
    }
    fputs(prefix, g_log);
    va_start(args, str);
    vfprintf(g_log, str, args);
    fflush(g_log);
    va_end(args);
  }
}
