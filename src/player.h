/* player.h: Declarations for player.c.
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

#ifndef PLAYER_H
#define PLAYER_H

#include "console.h"
#include "config.h"

enum directions
{
  DIR_DOWN = 1, DIR_LEFT, DIR_RIGHT, DIR_UP
};

typedef struct player
{
  int hp, armour, x, y, exp, score, stage, weapon, uplvl, dir;
  bool quit, inv;
  char ch;
} PLAYER;

PLAYER * create_player(void);
void ctrl_player(PLAYER * p);
void mv_player(PLAYER * p, int dir);
void set_player_dmg(PLAYER * p, int dmg);
void show_inventory(PLAYER * p);

#endif
