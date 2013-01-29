/* util.c: Helper functions for several small tasks
 * Copyright (C) 2011 - 2013 Weegee
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

// Splits the string str at the delimiter delim, returns the new string
char *
split_str(char * str, char delim)
{
  char * newstr, * ptr;

  if ((ptr = strchr(str, delim)) == NULL)
  {
    return NULL;
  }
  else
  {
    *ptr = '\0';
    newstr = ptr + 1;
  }

  return newstr;
}

// Trims the char str at the beginning/end, then returns the trimmed string
char *
trim_str(char * str, char totrim)
{
  int i;
  char * pos;

  // trim left side
  for (i = 0; str[i] == totrim; i++);
  pos = str;
  str = &str[i];
  // Move the string back to where it was
  memmove(pos, str, strlen(str) + 1);

  // trim right side
  for (i = (int) strlen(str) - 1; str[i] == totrim; i--);
  str[i + 1] = '\0';

  return str;
}

// Prints messages to the log file
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
