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
  p->hp = 100;
  p->score = 0;
  set_winchar(w_field, p->x, p->y, A_BOLD, CP_WHITEBLACK, p->ch);

  if (LOG_LEVEL >= LOG_VERBOSE)
  {
    set_winstr(w_game, 0, 0, A_NORMAL, CP_WHITEBLACK, "P: %02d|%02d", p->x,
               p->y);
  }
  write_log(LOG_VERBOSE, "Created player %p\n", (void *) p);
  write_log(LOG_DEBUG, "x: %d; y: %d; name: %s; hp: %d; score: %d\n", p->x,
            p->y, cfg->p_name, p->hp, p->score);
  return p;
}

// Controls the player input
void
ctrl_player(WINDOW * w_game, WINDOW * w_field, PLAYER * p)
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
    // Shoot
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
