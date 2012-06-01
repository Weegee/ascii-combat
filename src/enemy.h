/* enemy.h: Declarations for enemy.c.
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

#ifndef ENEMY_H
#define ENEMY_H

#include "console.h"
#include "obstacle.h"
#include "player.h"

#define EN_KAMCONST 0.1
#define EN_REGCONST 0.5

enum enemyinfo
{
  EN_MAX = 3
};

enum enemy_kamikaze_stats
{
  EN_KAMDAMAGE = 10, EN_KAMHEALTH = 5, EN_KAMSCORE = 25, EN_KAMMAXPROB = 25
};

enum enemy_regular_stats
{
  EN_REGDAMAGE = 5, EN_REGHEALTH = 5, EN_REGSCORE = 10, EN_REGMAXPROB = 50
};

typedef struct enemy
{
  int x, y, hp;
  chtype ch;
  enum e_type { EN_REG, EN_KAM } type;
  struct enemy * next;
} ENEMY;

typedef struct enemylist
{
  ENEMY * head, * tail;
  int num;
} ENEMYLIST;

void create_enemy(WINDOW * w_field, ENEMYLIST * le, int type);
ENEMYLIST * create_enemylist(void);
void ctrl_enemy_collision(WINDOW * w_field, ENEMYLIST * le);
void ctrl_enemy_kamikaze(WINDOW * w_field, ENEMYLIST * le, PLAYER * p);
void ctrl_enemy_spawn(WINDOW * w_field, ENEMYLIST * le, int time_elapsed);
void mv_enemy(WINDOW * w_field, ENEMY * e, int dir);
void rm_enemy(WINDOW * w_field, ENEMYLIST * le, ENEMY * e);
void rm_enemylist(ENEMYLIST * le);
void set_enemy_dmg(WINDOW * w_field, ENEMY * e, int dmg);

#endif
