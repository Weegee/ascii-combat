/* player.c: Input handling and other player-related functions.
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

#include "player.h"

// Creates a player bullet
/* TODO: Has to be rewritten from scratch. Other entities will have bullets too,
 * not only the player. Also, player bullets should always spawn in front of
 * the player and fly in the direction he is facing. */
void
create_bullet(WINDOW * w_field, BULLETLIST * lb, PLAYER * p)
{
  BULLET * b;

  b = malloc(sizeof(BULLET));
  b->x = p->x + 1;
  b->y = p->y;
  b->next = NULL;

  if (lb->tail != NULL)
  {
    lb->tail->next = b;
  }
  if (lb->num == 0)
  {
    lb->head = b;
  }
  lb->tail = b;
  p->ammo--;
  lb->num++;

  set_winchar(w_field, b->x, b->y, A_NORMAL, CP_WHITEBLACK, '-');
  g_fld[b->x][b->y] = ENT_BULLET;
  write_log(LOG_VERBOSE, "Created bullet %p\n", (void *) b);
  write_log(LOG_DEBUG, "x: %d; y: %d; lb->num: %d\n", b->x, b->y, lb->num);
}

// Initialises the player character
PLAYER *
create_player(WINDOW * w_game, WINDOW * w_field)
{
  PLAYER * p;

  p = malloc(sizeof(PLAYER));
  p->quit = false;
  p->ch = '>';
  p->x = CON_FIELDMAXX / 2;
  p->y = CON_FIELDMAXY / 2;
  g_fld[p->x][p->y] = ENT_PLAYER;
  p->hp = 50;
  p->ammo = 25;
  p->score = 0;
  set_winchar(w_field, p->x, p->y, A_BOLD, CP_WHITEBLACK, p->ch);

  if (LOG_LEVEL >= LOG_VERBOSE)
  {
    set_winstr(w_game, 0, 0, A_NORMAL, CP_WHITEBLACK, "P: %02d|%02d", p->x,
               p->y);
  }
  write_log(LOG_VERBOSE, "Created player %p\n", (void *) p);
  write_log(LOG_DEBUG, "x: %d; y: %d; name: %s; hp: %d; ammo: %d; score: %d\n",
            p->x, p->y, cfg->p_name, p->hp, p->ammo, p->score);
  return p;
}

// Controls the player bullet movement
void
ctrl_bullets(WINDOW * w_field, BULLETLIST * lb)
{
  if (lb->num > 0)
  {
    BULLET * b, * b_next;

    for (b = lb->head; b != NULL; b = b_next)
    {
      if (b->x > CON_FIELDMAXX)
      {
        write_log(LOG_DEBUG, "Bullet %p is outside the playing field; x: %d; "
                  "y: %d\n", (void *) b, b->x, b->y);
        b_next = b->next;
        rm_bullet(w_field, lb, b);
      }
      else
      {
        mv_bullet(w_field, b);
        b_next = b->next;
      }
    }
  }
}

// Controls the player input
void
ctrl_player(WINDOW * w_game, WINDOW * w_field, BULLETLIST * lb, PLAYER * p)
{
  char input;

  set_inputmode(IM_PLAYING);
  if (g_fld[p->x][p->y] != ENT_PLAYER)
  {
    g_fld[p->x][p->y] = ENT_PLAYER;
  }

  input = (char) getch();
  if (input == cfg->up)
  {
    p->ch = '^';
    if (p->y > CON_FIELDMINY)
    {
      mv_player(w_game, w_field, p, DIR_UP);
    }
    else
    {
      set_winchar(w_field, p->x, p->y, A_BOLD, CP_WHITEBLACK, p->ch);
    }
  }
  else if (input == cfg->down)
  {
    p->ch = 'v';
    if (p->y < CON_FIELDMAXY)
    {
      mv_player(w_game, w_field, p, DIR_DOWN);
    }
    else
    {
      set_winchar(w_field, p->x, p->y, A_BOLD, CP_WHITEBLACK, p->ch);
    }
  }
  else if (input == cfg->left)
  {
    p->ch = '<';
    if (p->x > CON_FIELDMINX)
    {
      mv_player(w_game, w_field, p, DIR_LEFT);
    }
    else
    {
      set_winchar(w_field, p->x, p->y, A_BOLD, CP_WHITEBLACK, p->ch);
    }
  }
  else if (input == cfg->right)
  {
    p->ch = '>';
    if (p->x < CON_FIELDMAXX)
    {
      mv_player(w_game, w_field, p, DIR_RIGHT);
    }
    else
    {
      set_winchar(w_field, p->x, p->y, A_BOLD, CP_WHITEBLACK, p->ch);
    }
  }
  else if (input == cfg->use)
  {
    if (p->x < CON_FIELDMAXX && p->ammo > 0)
    {
      if (g_fld[p->x + 1][p->y] != ENT_BULLET)
      {
        create_bullet(w_field, lb, p);
      }
    }
  }
  else if (input == '\n')
  {
    p->quit = true;
  }
  else if (input == 'p') // Just for debug purposes, will be removed later on
  {
    int t_freeze;

    t_freeze = pause_game();
    set_inputmode(IM_KEYPRESS);
    while(getch() != '\n');
    resume_game(t_freeze);
  }
  else if (input == ERR)
  {
    /* Since we use nodelay(), getch() returns ERR when no key is pressed
     * (instead of blocking everything else), however this leads to 100%
     * cpu usage. Thus, usleep is used to calm down the cpu. */
    usleep(500);
  }
}

// Initialises the bullet list
BULLETLIST *
create_bulletlist()
{
  BULLETLIST * lb;

  lb = malloc(sizeof(BULLETLIST));
  lb->head = NULL;
  lb->tail = NULL;
  lb->num = 0;
  write_log(LOG_DEBUG, "Created the bullet list %p\n", (void *) lb);
  return lb;
}

// Moves the player bullets on the playing field
void
mv_bullet(WINDOW * w_field, BULLET * b)
{
  write_log(LOG_VERBOSE, "Moving bullet %p\n", (void *) b);
  write_log(LOG_DEBUG, "x: %d; y: %d\n", b->x, b->y);

  if (g_fld[b->x][b->y] == ENT_BULLET)
  {
    set_winchar(w_field, b->x, b->y, A_NORMAL, CP_WHITEBLACK, ' ');
    g_fld[b->x][b->y] = ENT_NOTHING;
  }

  b->x++;
  if (b->x <= CON_FIELDMAXX)
  {
    if (g_fld[b->x][b->y] == ENT_NOTHING)
    {
      set_winchar(w_field, b->x, b->y, A_NORMAL, CP_WHITEBLACK, '-');
    }
    g_fld[b->x][b->y] = ENT_BULLET;
  }
}

// Moves the player on the playing field
void
mv_player(WINDOW * w_game, WINDOW * w_field, PLAYER * p, int dir)
{
  write_log(LOG_DEBUG, "Moving player %p; x: %d; y: %d\n", (void *) p, p->x,
            p->y);
  set_winchar(w_field, p->x, p->y, A_NORMAL, CP_WHITEBLACK, ' ');
  g_fld[p->x][p->y] = ENT_NOTHING;

  switch (dir)
  {
    case DIR_UP:
      p->y--;
      break;
    case DIR_DOWN:
      p->y++;
      break;
    case DIR_LEFT:
      p->x--;
      break;
    case DIR_RIGHT:
      p->x++;
      break;
    default:
      break;
  }
  g_fld[p->x][p->y] = ENT_PLAYER;
  set_winchar(w_field, p->x, p->y, A_BOLD, CP_WHITEBLACK, p->ch);

  if (LOG_LEVEL >= LOG_VERBOSE)
  {
    set_winstr(w_game, 0, 0, A_NORMAL, CP_WHITEBLACK, "P: %02d|%02d", p->x,
               p->y);
  }
}

// Destroys a player bullet
void
rm_bullet(WINDOW * w_field, BULLETLIST * lb, BULLET * b)
{
  BULLET * b_prev, * b_cur;

  write_log(LOG_VERBOSE, "Removing bullet %p\n", (void *) b);
  write_log(LOG_DEBUG, "x: %d; y: %d; lb->num: %d\n", b->x, b->y, lb->num);
  /* Only set a whitespace on the screen when the bullet was on the playing
   * field */
  if (b->x <= CON_FIELDMAXX)
  {
    if (g_fld[b->x][b->y] == ENT_BULLET)
    {
      set_winchar(w_field, b->x, b->y, A_NORMAL, CP_WHITEBLACK, ' ');
      g_fld[b->x][b->y] = ENT_NOTHING;
    }
  }

  for (b_cur = lb->head, b_prev = NULL; b_cur != NULL; b_prev = b_cur,
       b_cur = b_cur->next)
  {
    if (b_cur == b)
    {
      if (b == lb->head && b == lb->tail)
      {
        lb->head = NULL;
        lb->tail = NULL;
      }
      else if (b == lb->head)
      {
        lb->head = b->next;
      }
      else if (b == lb->tail)
      {
        lb->tail = b_prev;
        b_prev->next = NULL;
      }
      else
      {
        b_prev->next = b_cur->next;
      }

      b->next = NULL;
      free(b);
      b = NULL;
      lb->num--;
      break;
    }
  }

  write_log(LOG_DEBUG, "lb->num is now %d\n", lb->num);
}

// Destroys the bullet list and removes all remaining bullets
void
rm_bulletlist(BULLETLIST * lb)
{
  BULLET * b_cur;
  BULLET * b_next;

  write_log(LOG_DEBUG, "Removing the bullet list %p; lb->num: %d\n",
            (void *) lb, lb->num);
  b_cur = lb->head;
  while (b_cur != NULL)
  {
    b_next = b_cur->next;
    free(b_cur);
    b_cur = b_next;
  }
  free(lb);
}

// Sets player damage, shows a damage "animation"
// TODO: The "animation" should disappear after one second
void
set_player_dmg(WINDOW * w_field, PLAYER * p, int dmg)
{
  write_log(LOG_DEBUG, "Player %p receives %d damage; p->hp: %d\n", (void *) p,
            p->hp);
  p->hp -= dmg;
  p->hp = p->hp < 0 ? 0 : p->hp;
  write_log(LOG_DEBUG, "p->hp is now %d\n", p->hp);
  set_winchar(w_field, p->x, p->y, A_BOLD, CP_WHITERED, p->ch);
}
