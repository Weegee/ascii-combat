/* score.h: Declarations for score.c.
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

#ifndef SCORE_H
#define SCORE_H

#include <pwd.h>

#include "console.h"

enum scoreinfo
{
  SCORESIZE = 10
};

typedef struct scores
{
  int score;
  char name[P_MAXNAMELEN];
} SCORES;

int cmp_scores(const void * sc1, const void * sc2);
void ctrl_highscore(int p_exp, int sec_elapsed);
char * get_highscore_path(void);
SCORES * read_highscore(void);
void show_highscore(void);
void write_highscore(SCORES * sc);

#endif
