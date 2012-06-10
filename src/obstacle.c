/* obstacle.c: Obstacle-related functions.
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

#include "obstacle.h"

// Controls all obstacles (initiates the spawn/movement of obstacles)
void
ctrl_obstacles(WINDOW * w_field, OBSTACLELIST * lo, int time_elapsed)
{
  if (lo->num > 0)
  {
    OBSTACLE * o, * o_next;

    for (o = lo->head; o != NULL; o = o_next)
    {
      if (o->x < CON_FIELDMINX)
      {
        write_log(LOG_DEBUG, "Obstacle %p is outside the playing field; x: %d; "
                  "y: %d\n", (void *) o, o->x, o->y);
        o_next = o->next;
        rm_obstacle(w_field, lo, o);
      }
      else
      {
        mv_obstacle(w_field, o);
        o_next = o->next;
      }
    }
  }

  if (lo->num < OB_MAX)
  {
    int prob_reg;

    prob_reg = (int) ((time_elapsed / 1.2) * OB_REGCONST);
    prob_reg = prob_reg > OB_REGMAXPROB ? OB_REGMAXPROB : prob_reg;
    write_log(LOG_VERBOSE, "prob_reg is %d\n", prob_reg);
    if (get_randint(1, 100) <= prob_reg)
    {
      create_obstacle(w_field, lo, OB_REG);
    }
  }
}

// Creates a new obstacle
void
create_obstacle(WINDOW * w_field, OBSTACLELIST * lo, int type)
{
  OBSTACLE * o;

  o = malloc(sizeof(OBSTACLE));
  o->x = CON_FIELDMAXX;
  do
  {
    o->y = get_randint(CON_FIELDMINY, CON_FIELDMAXY);
  }
  while (g_fld[o->x][o->y] == ENT_OBSTACLE);

  switch (type)
  {
    case OB_REG:
      /* Regular obstacles have 10 different characters: # $ % & ( ) * + - /
       * Because it's rather difficult to see in which line the characters
       * ' , . are "flying", they have to be filtered out */
      do
      {
        o->ch = (chtype) get_randint(35, 47);
      }
      while (o->ch == '\'' || o->ch == ',' || o->ch == '.');
      o->hp = OB_REGHEALTH;
      set_winchar(w_field, o->x, o->y, A_NORMAL, CP_WHITEBLACK, o->ch);
      break;
    default:
      break;
  }
  o->type = type;

  o->next = NULL;
  if (lo->tail != NULL)
  {
    lo->tail->next = o;
  }
  if (lo->num == 0)
  {
    lo->head = o;
  }
  lo->tail = o;
  lo->num++;

  g_fld[o->x][o->y] = ENT_OBSTACLE;
  write_log(LOG_VERBOSE, "Created obstacle %p\n", (void *) o);
  write_log(LOG_DEBUG, "x: %d; y: %d; type: %d; ch: %c; lo->num: %d\n", o->x,
            o->y, o->type, o->ch, lo->num);
}

// Creates the obstacle list
OBSTACLELIST *
create_obstaclelist()
{
  OBSTACLELIST * lo;

  lo = malloc(sizeof(OBSTACLELIST));
  lo->head = NULL;
  lo->tail = NULL;
  lo->num = 0;
  write_log(LOG_DEBUG, "Created the obstacle list %p\n", (void *) lo);
  return lo;
}

// Moves the obstacles on the playing field
void
mv_obstacle(WINDOW * w_field, OBSTACLE * o)
{
  write_log(LOG_VERBOSE, "Moving obstacle %p\n", (void *) o);
  write_log(LOG_DEBUG, "x: %d; y: %d; type: %d\n", o->x, o->y, o->type);

  if (g_fld[o->x][o->y] == ENT_OBSTACLE || g_fld[o->x][o->y] == ENT_NOTHING)
  {
    set_winchar(w_field, o->x, o->y, A_NORMAL, CP_WHITEBLACK, ' ');
    g_fld[o->x][o->y] = ENT_NOTHING;
  }

  o->x--;
  if (o->x >= CON_FIELDMINX)
  {
    set_winchar(w_field, o->x, o->y, A_NORMAL, CP_WHITEBLACK, o->ch);
    g_fld[o->x][o->y] = ENT_OBSTACLE;
  }
}

// Destroys the specified obstacle
void
rm_obstacle(WINDOW * w_field, OBSTACLELIST * lo, OBSTACLE * o)
{
  OBSTACLE * o_prev;
  OBSTACLE * o_cur;

  write_log(LOG_VERBOSE, "Removing obstacle %p\n", (void *) o);
  write_log(LOG_DEBUG, "x: %d; y: %d; type: %d; ch: %c; lo->num: %d\n", o->x,
            o->y, o->type, o->ch, lo->num);

  /* Only set a whitespace on the screen if the obstacle was on the playing
   * field */
  if (o->x >= CON_FIELDMINX)
  {
    if (g_fld[o->x][o->y] == ENT_OBSTACLE)
    {
      set_winchar(w_field, o->x, o->y, A_NORMAL, CP_WHITEBLACK, ' ');
      g_fld[o->x][o->y] = ENT_NOTHING;
    }
  }

  for (o_cur = lo->head, o_prev = NULL; o_cur != NULL; o_prev = o_cur,
       o_cur = o_cur->next)
  {
    if (o_cur == o)
    {
      if (o == lo->head && o == lo->tail)
      {
        lo->head = NULL;
        lo->tail = NULL;
      }
      else if (o == lo->head)
      {
        lo->head = o->next;
      }
      else if (o == lo->tail)
      {
        lo->tail = o_prev;
        o_prev->next = NULL;
      }
      else
      {
        o_prev->next = o_cur->next;
      }

      free(o);
      o = NULL;
      lo->num--;
      break;
    }
  }

  write_log(LOG_DEBUG, "lo->num is now %d\n", lo->num);
}

// Destroys the obstacle list and all remaining obstacles
void
rm_obstaclelist(OBSTACLELIST * lo)
{
  OBSTACLE * o_cur;
  OBSTACLE * o_next;

  write_log(LOG_DEBUG, "Removing the obstacle list %p; lo->num: %d\n",
            (void *) lo, lo->num);
  o_cur = lo->head;
  while (o_cur != NULL)
  {
    o_next = o_cur->next;
    free(o_cur);
    o_cur = o_next;
  }
  free(lo);
}

// Sets the obstacle damage, shows a damage "animation"
void
set_obstacle_dmg(WINDOW * w_field, OBSTACLE * o, int dmg)
{
  write_log(LOG_DEBUG, "Obstacle %p receives %d damage; o->hp: %d\n",
            (void *) o, o->hp);
  o->hp -= dmg;
  o->hp = o->hp < 0 ? 0 : o->hp;
  write_log(LOG_DEBUG, "o->hp is now %d\n", o->hp);
  set_winchar(w_field, o->x, o->y, A_NORMAL, CP_WHITERED, o->ch);
}
