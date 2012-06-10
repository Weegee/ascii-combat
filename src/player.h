/* player.h: Declarations for player.c.
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

#ifndef PLAYER_H
#define PLAYER_H

#include "console.h"

#define BU_PLDAMAGE 5

enum directions
{
  DIR_DOWN = 1, DIR_LEFT, DIR_RIGHT, DIR_UP
};

typedef struct bullet
{
  int x, y;
  struct bullet * next;
} BULLET;

typedef struct bulletlist
{
  BULLET * head, * tail;
  int num;
} BULLETLIST;

typedef struct player
{
  int ammo, hp, x, y, score;
  bool quit;
  chtype ch;
} PLAYER;

void create_bullet(WINDOW * w_field, BULLETLIST * lb, PLAYER * p);
PLAYER * create_player(WINDOW * w_game, WINDOW * w_field);
void ctrl_bullets(WINDOW * w_field, BULLETLIST * lb);
void ctrl_player(WINDOW * w_game, WINDOW * w_field, BULLETLIST * lb, PLAYER * p);
void rm_bullet(WINDOW * w_field, BULLETLIST * lb, BULLET * b);
void rm_bulletlist(BULLETLIST * lb);
BULLETLIST * create_bulletlist(void);
void mv_bullet(WINDOW * w_field, BULLET * b);
void mv_player(WINDOW * w_game, WINDOW * w_field, PLAYER * p, int dir);
void pause_game(void);
void set_player_dmg(WINDOW * w_field, PLAYER * p, int dmg);

#endif
