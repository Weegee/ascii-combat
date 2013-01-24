/* game.h: Declarations for game.c.
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

#ifndef GAME_H
#define GAME_H

#include <ctype.h>
#include <sys/time.h>
#include <time.h>

#include "score.h"
#include "player.h"

typedef struct timer
{
  int start, sec_elapsed;
  long msec_elapsed;
} TIMER;

extern TIMER * t;

void ctrl_timer(WINDOW * w_game);
void exit_game(void);
void init_game(void);
void init_timer(WINDOW * w);
int loop_game(WINDOWLIST * lw, PLAYER * p);
int pause_game(void);
void quit_game(WINDOWLIST * lw, PLAYER * p);
void resume_game(int t_freeze);
void run_game(void);
void show_message(const char * msg, ...);
void show_options(void);
bool show_prompt(const char * msg, ...);
void show_splashscreen(void);
void show_startmenu(void);
void update_status_window(WINDOW * w_status, PLAYER * p);

#endif
