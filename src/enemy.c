/* enemy.c: Enemy AI and other enemy-related functions.
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

#include "enemy.h"

// Creates an enemy character
void
create_enemy(WINDOW * w_field, ENEMYLIST * le, int type)
{
  ENEMY * e;

  e = malloc(sizeof(ENEMY));
  e->x = CON_FIELDMAXX;
  // Prevent spawning enemies on other enemies
  do
  {
    e->y = get_randint(CON_FIELDMINY, CON_FIELDMAXY);
  }
  while (g_fld[e->x][e->y] == ENT_ENEMY);

  switch (type)
  {
    case EN_REG:
      e->ch = '<';
      set_winchar(w_field, e->x, e->y, A_BOLD, CP_YELLOWBLACK, e->ch);
      e->hp = EN_REGHEALTH;
      break;
    case EN_KAM:
      e->ch = '!';
      set_winchar(w_field, e->x, e->y, A_BOLD, CP_MAGENTABLACK, e->ch);
      e->hp = EN_KAMHEALTH;
      break;
    default:
      break;
  }
  e->type = type;

  e->next = NULL;
  if (le->tail != NULL)
  {
    le->tail->next = e;
  }
  if (le->num == 0)
  {
    le->head = e;
  }
  le->tail = e;
  le->num++;

  g_fld[e->x][e->y] = ENT_ENEMY;
  write_log(LOG_VERBOSE, "Created enemy %p\n", (void *) e);
  write_log(LOG_DEBUG, "x: %d; y: %d; type: %d; ch: %c; le->num: %d\n", e->x,
            e->y, e->type, e->ch, le->num);
}

// Initialises the enemy list
ENEMYLIST *
create_enemylist()
{
  ENEMYLIST * le;

  le = malloc(sizeof(ENEMYLIST));
  le->head = NULL;
  le->tail = NULL;
  le->num = 0;
  write_log(LOG_DEBUG, "Created the enemy list %p\n", (void *) le);
  return le;
}

// Checks for collisions between enemies
/* TODO: This only works for two enemies at a time, as the, for example, third
 * enemy, which should have also been removed (because it was also involved in
 * the collision), doesn't "know" about the two other enemies (as it is the only
 * one left in the enemy list after the other ones have been removed). Maybe the
 * total number of enemies on the playing field should be used to rewrite a
 * better collision check function. However, it is more likely that the whole
 * collision check thingy needs to be rewritten ... */
void
ctrl_enemy_collision(WINDOW * w_field, ENEMYLIST * le)
{
  ENEMY * e1, * e2;

  for (e1 = le->head; e1 != NULL; e1 = e1->next)
  {
    for (e2 = e1->next; e2 != NULL; e2 = e2->next)
    {
      if (e1->x == e2->x && e1->y == e2->y)
      {
        int x, y;

        x = e1->x;
        y = e1->y;
        write_log(LOG_INFO, "Enemy %p collided with enemy %p at (%d|%d)\n",
                  (void *) e1, (void *) e2, x, y);
        rm_enemy(w_field, le, e2);
        rm_enemy(w_field, le, e1);
        /* TODO: The yellow '*' on the playing field is a nice way to show that
         * a collision has occured, however it would be better if it just
         * disappeared after one second. Might be annoying to find a better
         * solution though since the collision checks are rather nested
         * functions ... This TODO applies to all other occurences of the yellow
         * '*'. */
        set_winchar(w_field, x, y, A_BOLD, CP_YELLOWBLACK, '*');
      }
    }
  }
}

// Controls kamikaze enemies (kamikaze AI)
void
ctrl_enemy_kamikaze(WINDOW * w_field, ENEMYLIST * le, PLAYER * p)
{
  ENEMY * e;

  for (e = le->head; e != NULL; e = e->next)
  {
    if (e->type == EN_KAM)
    {
      // TODO: Shouldn this rather be part of a general ctrl_enemy function?
      if (e->x < CON_FIELDMINX || e->x > CON_FIELDMAXX || e->y < CON_FIELDMINY
          || e->y > CON_FIELDMAXY)
      {
        write_log(LOG_DEBUG, "Enemy %p is outside the playing field; x: %d; "
                  "y: %d\n", (void *) e, e->x, e->y);
        rm_enemy(w_field, le, e);
      }
      else
      {
        /* TODO:
        * - Write an AI which is able to dodge obstacles, bullets and other
        *   enemies to avoid crashes
        * - The direction which is chosen by the enemy should depend on other
        *   entities' positions on the playing field
        * - To receive information about entities all around the enemy the
        *   playing field should be checked
        * - Write functions to abstract the position checks
        * Current AI just follows the player and isn't aware of other entities,
        * the enemy just crashes into them */
        int dir, xdiff, ydiff;

        dir = false;
        xdiff = p->x - e->x;
        ydiff = p->y - e->y;
        /* The enemy chooses its way depending on the horizontal and vertical
         * distance between him and the player
         * If the distances are the same, the direction is decided "by toss" */
        if (abs(xdiff) > abs(ydiff))
        {
          if (xdiff < 0)
          {
            dir = DIR_LEFT;
          }
          else if (xdiff > 0)
          {
            dir = DIR_RIGHT;
          }
        }
        else if (abs(xdiff) < abs(ydiff))
        {
          if (ydiff < 0)
          {
            dir = DIR_UP;
          }
          else if (ydiff > 0)
          {
            dir = DIR_DOWN;
          }
        }
        else
        {
          if (get_randint(0, 1) == 0)
          {
            if (xdiff < 0)
            {
              dir = DIR_LEFT;
            }
            else if (xdiff > 0)
            {
              dir = DIR_RIGHT;
            }
          }
          else
          {
            if (ydiff < 0)
            {
              dir = DIR_UP;
            }
            else if (ydiff > 0)
            {
              dir = DIR_DOWN;
            }
          }
        }

        write_log(LOG_DEBUG, "Enemy %p xdiff: %d; ydiff: %d; dir: %d\n",
                  (void *) e, xdiff, ydiff, dir);
        if (dir)
        {
          mv_enemy(w_field, e, dir);
          ctrl_enemy_collision(w_field, le);
        }
      }
    }
  }
}

// Controls the spawn of enemies
void
ctrl_enemy_spawn(WINDOW * w_field, ENEMYLIST * le, int time_elapsed)
{
  if (le->num < EN_MAX)
  {
    int prob_kam;

    prob_kam = (int) ((time_elapsed / 1.2) * EN_KAMCONST);
    prob_kam = prob_kam > EN_KAMMAXPROB ? EN_KAMMAXPROB : prob_kam;
    write_log(LOG_VERBOSE, "prob_kam is %d\n", prob_kam);
    if (get_randint(1, 100) <= prob_kam)
    {
      create_enemy(w_field, le, EN_KAM);
    }
  }
}

// Moves an enemy on the playing field
void
mv_enemy(WINDOW * w_field, ENEMY * e, int dir)
{
  write_log(LOG_VERBOSE, "Moving enemy %p\n", (void *) e);
  write_log(LOG_DEBUG, "x: %d; y: %d; dir: %d; type: %d\n", e->x, e->y, dir,
            e->type);

  if (g_fld[e->x][e->y] == ENT_ENEMY)
  {
    set_winchar(w_field, e->x, e->y, A_NORMAL, CP_WHITEBLACK, ' ');
    g_fld[e->x][e->y] = ENT_NOTHING;
  }

  switch (dir)
  {
    case DIR_UP:
      e->y--;
      break;
    case DIR_DOWN:
      e->y++;
      break;
    case DIR_LEFT:
      e->x--;
      break;
    case DIR_RIGHT:
      e->x++;
      break;
    default:
      break;
  }

  if (e->x >= CON_FIELDMINX && e->x <= CON_FIELDMAXX && e->y >= CON_FIELDMINY
      && e->y <= CON_FIELDMAXY)
  {
    switch (e->type)
    {
      case EN_REG:
        set_winchar(w_field, e->x, e->y, A_BOLD, CP_YELLOWBLACK, e->ch);
        break;
      case EN_KAM:
        set_winchar(w_field, e->x, e->y, A_BOLD, CP_MAGENTABLACK, e->ch);
        break;
      default:
        break;
    }
    g_fld[e->x][e->y] = ENT_ENEMY;
  }
}

// Destroys an enemy character
void
rm_enemy(WINDOW * w_field, ENEMYLIST * le, ENEMY * e)
{
  ENEMY * e_prev;
  ENEMY * e_cur;

  write_log(LOG_VERBOSE, "Removing enemy %p\n", (void *) e);
  write_log(LOG_DEBUG, "x: %d; y: %d; type: %d; ch: %c; le->num: %d\n", e->x,
            e->y, e->type, e->ch, le->num);

  if (g_fld[e->x][e->y] == ENT_ENEMY)
  {
    set_winchar(w_field, e->x, e->y, A_NORMAL, CP_WHITEBLACK, ' ');
    g_fld[e->x][e->y] = ENT_NOTHING;
  }

  for (e_cur = le->head, e_prev = NULL; e_cur != NULL; e_prev = e_cur,
       e_cur = e_cur->next)
  {
    if (e_cur == e)
    {
      if (e == le->head && e == le->tail)
      {
        le->head = NULL;
        le->tail = NULL;
      }
      else if (e == le->head)
      {
        le->head = e->next;
      }
      else if (e == le->tail)
      {
        le->tail = e_prev;
        e_prev->next = NULL;
      }
      else
      {
        e_prev->next = e_cur->next;
      }

      free(e);
      le->num--;
      break;
    }
  }

  write_log(LOG_DEBUG, "le->num is now %d\n", le->num);
}

// Removes the enemy list and all remaining enemies
void
rm_enemylist(ENEMYLIST * le)
{
  ENEMY * e_cur;
  ENEMY * e_next;

  e_cur = le->head;
  write_log(LOG_DEBUG, "Removing the enemy list %p; le->num: %d\n", (void *) le,
            le->num);
  while (e_cur != NULL)
  {
    e_next = e_cur->next;
    free(e_cur);
    e_cur = e_next;
  }
  free(le);
}

// Sets the enemy damage, shows a damage "animation"
void
set_enemy_dmg(WINDOW * w_field, ENEMY * e, int dmg)
{
  write_log(LOG_DEBUG, "Enemy %p receives %d damage; e->hp: %d\n", (void *) e,
            dmg, e->hp);
  e->hp -= dmg;
  e->hp = (e->hp < 0) ? 0 : e->hp;
  write_log(LOG_DEBUG, "e->hp is now %d\n", e->hp);
  set_winchar(w_field, e->x, e->y, A_BOLD, CP_WHITERED, e->ch);
}
