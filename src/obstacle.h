/* obstacle.h: Declarations for obstacle.c.
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

#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "console.h"

#define OB_REGCONST 1.0

enum obstacleinfo
{
  OB_MAX = 250
};

enum obstacle_regular_stats
{
  OB_REGDAMAGE = 5, OB_REGHEALTH = 5, OB_REGSCORE = 5, OB_REGMAXPROB = 100
};

typedef struct obstacle
{
  int x, y, hp;
  chtype ch;
  enum o_type { OB_REG } type;
  struct obstacle * next;
} OBSTACLE;

typedef struct obstaclelist
{
  OBSTACLE * head, * tail;
  int num;
} OBSTACLELIST;

void create_obstacle(WINDOW * w_field, OBSTACLELIST * lo, int type);
OBSTACLELIST * create_obstaclelist(void);
void ctrl_obstacles(WINDOW * w_field, OBSTACLELIST * lo, int time_elapsed);
void mv_obstacle(WINDOW * w_field, OBSTACLE * o);
void rm_obstacle(WINDOW * w_field, OBSTACLELIST * lo, OBSTACLE * o);
void rm_obstaclelist (OBSTACLELIST * lo);
void set_obstacle_dmg(WINDOW * w_field, OBSTACLE * o, int dmg);

#endif
