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
  OBSTACLELIST * lo;
  BULLETLIST * lb;
  ENEMYLIST * le;

  // Random numbers
  srand((unsigned)time(NULL));

  printf("ASCII Combat %s\nCopyright (C) 2011, 2012 Weegee\n", INFO_VERSION);
  printf("\nThis program comes with ABSOLUTELY NO WARRANTY.\n"
         "This is free software, and you are welcome to redistribute it\n"
         "under certain conditions; see the COPYING file for details.\n");

  init_console();
  init_config();
  init_game();
  init_field();
  lw = init_windows();
  p = create_player(lw->w_game, lw->w_field);
  init_timer(lw->w_game);
  lo = create_obstaclelist();
  lb = create_bulletlist();
  le = create_enemylist();
  while (loop_game(lw, lb, le, lo, p));

  quit_game(lw, lb, le, lo, p);
  return EXIT_SUCCESS;
}
