/* player.c: Input handling and other player-related functions.
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

#include "player.h"

// Initialises the player character
PLAYER *
create_player(void)
{
  PLAYER * p;

  p = malloc(sizeof(PLAYER));
  p->quit = false;
  p->inv = false;
  p->ch = '>';
  p->x = CON_FIELDMAXX / 2;
  p->y = CON_FIELDMAXY / 2;
  g_fld[p->x][p->y] = ENT_PLAYER;
  p->hp = 100;
  p->armour = 100;
  p->score = 0;
  p->exp = 0;
  set_winchar(lw->w_field, p->x, p->y, A_BOLD, CP_WHITEBLACK, p->ch);

  if (LOG_LEVEL >= LOG_VERBOSE)
  {
    set_winstr(lw->w_game, 0, 0, A_NORMAL, CP_WHITEBLACK, "P: %02d|%02d", p->x,
               p->y);
  }
  write_log(LOG_VERBOSE, "%s:\n\tCreated player %p\n", __func__, (void *) p);
  write_log(LOG_DEBUG, "\tx: %d\n\ty: %d\n\thp: %d\n\tscore: %d\n",
            p->x, p->y, p->hp, p->score);
  return p;
}

// Controls the player input
void
ctrl_player(PLAYER * p)
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
      mv_player(p, DIR_UP);
    }
    else
    {
      set_winchar(lw->w_field, p->x, p->y, A_BOLD, CP_WHITEBLACK, p->ch);
    }
  }
  else if (input == cfg->down)
  {
    p->ch = 'v';
    if (p->y < CON_FIELDMAXY)
    {
      mv_player(p, DIR_DOWN);
    }
    else
    {
      set_winchar(lw->w_field, p->x, p->y, A_BOLD, CP_WHITEBLACK, p->ch);
    }
  }
  else if (input == cfg->left)
  {
    p->ch = '<';
    if (p->x > CON_FIELDMINX)
    {
      mv_player(p, DIR_LEFT);
    }
    else
    {
      set_winchar(lw->w_field, p->x, p->y, A_BOLD, CP_WHITEBLACK, p->ch);
    }
  }
  else if (input == cfg->right)
  {
    p->ch = '>';
    if (p->x < CON_FIELDMAXX)
    {
      mv_player(p, DIR_RIGHT);
    }
    else
    {
      set_winchar(lw->w_field, p->x, p->y, A_BOLD, CP_WHITEBLACK, p->ch);
    }
  }
  else if (input == cfg->use)
  {
    set_player_dmg(lw->w_field, p, 10); // Just for testing purposes
  }
  else if (input == cfg->nextw)
  {
    // Next weapon
  }
  else if (input == cfg->prevw)
  {
    // Previous weapon
  }
  else if (input == cfg->inv)
  {
    p->inv = true;
  }
  else if (input == '\n')
  {
    p->quit = true;
  }
  else if (input == ERR)
  {
    /* Since we use nodelay(), getch() returns ERR when no key is pressed
     * (instead of blocking everything else), however this leads to 100%
     * cpu usage. Thus, usleep is used to calm down the cpu. */
    usleep(500);
  }

  if (p->hp == 0)
  {
    write_log(LOG_INFO, "%s:\n\tPlayer is dead, stopping game ...\n", __func__);
    p->quit = true;
  }
}

// Moves the player on the playing field
void
mv_player(PLAYER * p, int dir)
{
  write_log(LOG_DEBUG, "%s:\n\tMoving player %p\n\tOld x: %d\n\tOld y: %d\n",
            __func__, (void *) p, p->x, p->y);
  set_winchar(lw->w_field, p->x, p->y, A_NORMAL, CP_WHITEBLACK, ' ');
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
  set_winchar(lw->w_field, p->x, p->y, A_BOLD, CP_WHITEBLACK, p->ch);

  if (LOG_LEVEL >= LOG_VERBOSE)
  {
    set_winstr(lw->w_game, 0, 0, A_NORMAL, CP_WHITEBLACK, "P: %02d|%02d", p->x,
               p->y);
  }
  write_log(LOG_DEBUG, "\tNew x: %d\n\tNew y: %d\n", p->x, p->y);
}

// Sets player damage, shows a damage "animation"
// TODO: The "animation" should disappear after one second
void
set_player_dmg(WINDOW * w_field, PLAYER * p, int dmg)
{
  write_log(LOG_DEBUG, "%s:\n\tPlayer %p receives %d damage\n\tp->hp: %d"
            "\n\tp->armour: %d\n", __func__, (void *) p, dmg, p->hp, p->armour);
  if (p->armour > 0)
  {
    if (p->armour < 2 * dmg / 3)
    {
      // Make sure the player receives all damage
      p->armour -= 2 * dmg / 3;
      dmg += p->armour;
      p->hp -= dmg;
    }
    else
    {
      p->hp -= dmg / 3;
      p->armour -= 2 * dmg / 3;
    }
  }
  else
  {
    p->hp -= dmg;
  }
  p->hp = p->hp < 0 ? 0 : p->hp;
  p->armour = p->armour < 0 ? 0 : p->armour;
  write_log(LOG_DEBUG, "\tNew p->hp: %d\n\tp->armour: %d\n", p->hp, p->armour);
  set_winchar(w_field, p->x, p->y, A_BOLD, CP_WHITERED, p->ch);
}

// Shows the inventory
void
show_inventory(PLAYER * p)
{
  WINDOW * w_inv;
  COORDS co;

  w_inv = create_win(0, 0, 0, 0, 1, CP_WHITEBLUE);
  co = get_geometry(w_inv);
  set_winstr(w_inv, (co.x - (int) strlen("INVENTORY")) / 2, 1, A_BOLD,
             CP_WHITEBLUE, "INVENTORY");

  set_inputmode(IM_KEYPRESS);
  getch();
  rm_win(w_inv);
  p->inv = false;

  // w_inv overlapped all windows, so we have to redraw them
  redrawwin(lw->w_field);
  redrawwin(lw->w_game);
  redrawwin(lw->w_status);
  wrefresh(lw->w_field);
  wrefresh(lw->w_game);
  wrefresh(lw->w_status);
}
