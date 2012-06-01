/* util.c: Functions to facilitate common coding needs.
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
