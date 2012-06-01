/* main.c: Command line parsing.
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

#include "game.h"

int main(void)
{
  PLAYER * p;
  WINDOWLIST * lw;
  TIMER * t;
  OBSTACLELIST * lo;
  BULLETLIST * lb;
  ENEMYLIST * le;
  char p_name[P_MAXNAMELEN];

  // Random numbers
  srand((unsigned)time(NULL));

  printf("ASCII Combat %s\nCopyright (C) 2011, 2012 Weegee\n", INFO_VERSION);
  printf("\nThis program comes with ABSOLUTELY NO WARRANTY.\n"
         "This is free software, and you are welcome to redistribute it\n"
         "under certain conditions; see the COPYING file for details.\n");
  // TODO: The player should be able to do this via an options menu
  printf("\nEnter your name and press ENTER (max. %d characters)\n",
         P_MAXNAMELEN);
  fgets(p_name, P_MAXNAMELEN, stdin);
  if (p_name[0] == '\n')
  {
    strcpy(p_name, "Unknown");
  }
  else if (p_name[strlen(p_name) - 1] == '\n')
  {
    p_name[strlen(p_name) - 1] = '\0';
  }

  init_console();
  init_game();
  init_field();
  lw = init_windows();
  p = create_player(lw->w_game, lw->w_field, p_name);
  t = init_timer(lw->w_game);
  lo = create_obstaclelist();
  lb = create_bulletlist();
  le = create_enemylist();
  while (loop_game(lw, lb, le, lo, p, t));

  quit_game(lw, lb, le, lo, p, t);
  return EXIT_SUCCESS;
}
