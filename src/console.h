/* console.h: Declarations for console.c.
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

#ifndef CONSOLE_H
#define CONSOLE_H

#include <ncurses.h>
#include <menu.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
#include <stdbool.h>

#include "util.h"

#define INFO_VERSION "0.1.0"

enum loglevel
{
  LOG_NONE, LOG_INFO, LOG_VERBOSE, LOG_DEBUG, LOG_LEVEL = LOG_DEBUG
};

enum consoleinfo
{
  CON_FIELDMAXX = 77, CON_FIELDMAXY = 20, CON_FIELDMINX = 0, CON_FIELDMINY = 0,
  CON_TERMX = 80, CON_TERMY = 25
};

enum inputmodes
{
  IM_TEXTINPUT, IM_KEYPRESS, IM_PLAYING
};

enum colourpairs
{
  CP_WHITEBLACK, CP_WHITERED, CP_REDBLACK, CP_YELLOWBLACK, CP_MAGENTABLACK,
  CP_REDWHITE
};

enum entitytypes
{
  ENT_NOTHING, ENT_PLAYER, ENT_OBSTACLE, ENT_ENEMY, ENT_BULLET
};

typedef struct coordinates
{
  int x, y;
} COORDS;

typedef struct windowlist
{
  WINDOW * w_ammo, * w_game, * w_field, * w_hp, * w_score;
} WINDOWLIST;

typedef struct timer
{
  int start, sec_elapsed;
  long msec_elapsed;
} TIMER;

extern FILE * g_log;
extern int g_fld[CON_FIELDMAXX + 1][CON_FIELDMAXY + 1];

MENU * create_menu(WINDOW * w_menu, WINDOW * w_sub, const char ** items,
                   int num, chtype cp_sel, chtype cp_unsel);
WINDOW * create_subwin(WINDOW * w_parent, int rows, int cols, int x, int y,
                       bool box, chtype cp);
WINDOW * create_win(int rows, int cols, int x, int y, bool box, chtype cp);
void ctrl_timer(WINDOW * w_game, TIMER * t);
COORDS get_geometry(WINDOW * w);
void init_console(void);
void init_field(void);
TIMER * init_timer(WINDOW * w);
WINDOWLIST * init_windows(void);
void rm_menu(MENU * m);
void rm_win(WINDOW * w);
void set_inputmode(int mode);
void set_winchar(WINDOW * w, int x, int y, attr_t a, short cp, chtype ch);
void set_winstr(WINDOW * w, int x, int y, attr_t a, short cp, const char * str,
                ...);
void write_log(int level, const char * str, ...);

#endif
