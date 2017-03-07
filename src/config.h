/* config.h: Declarations for config.c.
 * Copyright (C) 2011 - 2017 Weegee
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

#ifndef CONFIG_H
#define CONFIG_H

#include <pwd.h>
#include <ctype.h>

#include "util.h"

typedef struct configuration
{
  unsigned char up, down, left, right, use, next, prev, inv;
} CONFIG;

extern CONFIG * cfg;

char * get_config_path(void);
void init_config(void);
void parse_config(FILE * f_cfg);
void read_config(void);
void write_config(void);

#endif
