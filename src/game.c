/* game.c: Main game events and game control.
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

#include "game.h"

TIMER * t;

// Displays the elapsed seconds since the game's start, DEBUG ONLY
void
ctrl_timer(void)
{
  if (LOG_LEVEL >= LOG_VERBOSE)
  {
    set_winstr(lw->w_game, 17, 0, A_NORMAL, CP_WHITEBLACK, "T: %d",
               t->sec_elapsed);
  }
}

// Clears all essential data (config, log file), ends ncurses
void
exit_game(void)
{
  endwin();
  _free(cfg);
  write_log(LOG_INFO, "%s:\n\tQuitting game, goodbye!\n", __func__);
  fclose(g_log);
  g_log = NULL;
}

// Shows the start screen, then switches to the main menu
void
init_game(void)
{
  show_splashscreen();
  show_startmenu();
}

// Initialises the timer
void
init_timer(void)
{
  struct timeval ct;

  t = malloc(sizeof(TIMER));
  gettimeofday(&ct, NULL);
  t->start = (int) ct.tv_sec;
  t->msec_elapsed = 0;
  t->sec_elapsed = 0;

  ctrl_timer();
  write_log(LOG_INFO, "%s:\n\tGame started at %d\n", __func__, t->start);
  write_log(LOG_DEBUG, "\tCurrent time: %d.%d\n", ct.tv_sec, ct.tv_usec);
}

// Main game loop, calls all control functions after a certain time
int
loop_game(PLAYER * p)
{
  struct timeval ct;
  long msec_elapsed;
  int retval;

  retval = 1;
  /* The game runs in an infinite loop which executes different functions over
   * and over again after a set amount of time */
  gettimeofday(&ct, NULL);
  msec_elapsed = (long) (((ct.tv_sec - t->start) * 1000) + (ct.tv_usec / 1000));
  if (msec_elapsed % 100 == 0 && msec_elapsed != t->msec_elapsed)
  {
    /* Store the last "run" in t->msec_elapsed to prevent multiple execution of
     * the functions in one millisecond */
    t->msec_elapsed = msec_elapsed;
    t->sec_elapsed = (int) (t->msec_elapsed / 1000);

    if (msec_elapsed % 1000 == 0)
    {
      ctrl_timer();

      if (msec_elapsed % 10000 == 0)
      {
        p->stage++; // TODO
        set_winstr(lw->w_game, 10, 0, A_NORMAL, CP_WHITEBLACK, "S: %02d",
                   p->stage);
      }
    }
  }

  ctrl_player(p);
  if (p->quit)
  {
    write_log(LOG_INFO, "%s:\n\tPlayer wants to quit, stopping ...\n", __func__);
    retval = 0;
  }

  if (p->inv)
  {
    int t_freeze;

    t_freeze = pause_game();
    show_inventory(p);
    resume_game(t_freeze);
  }

  return retval;
}

// Pauses the game by freezing the timer
int
pause_game(void)
{
  struct timeval ct;
  int t_freeze;

  set_inputmode(IM_KEYPRESS);
  gettimeofday(&ct, NULL);
  t_freeze = (int) ct.tv_sec;
  write_log(LOG_INFO, "%s:\n\tGame paused at %d\n", __func__, t_freeze);
  write_log(LOG_DEBUG, "\tCurrent time: %d.%d\n", ct.tv_sec, ct.tv_usec);
  return t_freeze;
}

// Removes all entities, lists and windows
void
quit_game(PLAYER * p)
{
  rm_win(lw->w_field);
  rm_win(lw->w_game);
  rm_win(lw->w_status);

  ctrl_highscore(p->exp, t->sec_elapsed);
  show_highscore();

  _free(p);
  _free(lw);
  _free(t);
}

// Resumes the game
void
resume_game(int t_freeze)
{
  struct timeval ct;

  gettimeofday(&ct, NULL);
  t->start = t->start + ((int) ct.tv_sec - t_freeze);
  write_log(LOG_INFO, "%s:\n\tGame resumed at %d\n", __func__, ct.tv_sec);
  write_log(LOG_VERBOSE, "\tt->start: %d\n", t->start);
  write_log(LOG_DEBUG, "\tCurrent time: %d.%d\n", ct.tv_sec, ct.tv_usec);
}

// Initialises the playing field and all entities
void
run_game(void)
{
  PLAYER * p;

  init_field();
  init_windows();
  p = create_player();
  p->stage = 1;
  set_winstr(lw->w_game, 10, 0, A_NORMAL, CP_WHITEBLACK, "S: %02d", p->stage);
  init_timer();
  init_status_window(p);
  while (loop_game(p));
  quit_game(p);
}

// Displays text in a message window (max. 160 characters)
void
show_message(const char * msg, ...)
{
  va_list args;
  WINDOW * w_msg;
  int t_freeze;

  w_msg = create_win(2, CON_TERMX, 0, CON_TERMY - 2, false, CP_BLACKWHITE);
  va_start(args, msg);
  wmove(w_msg, 0, 0);
  vw_printw(w_msg, msg, args);
  va_end(args);
  wrefresh(w_msg);

  t_freeze = pause_game();
  set_inputmode(IM_KEYPRESS);
  while (getch() != '\n');
  resume_game(t_freeze);
  wbkgdset(w_msg, CP_WHITEBLACK);
  rm_win(w_msg);

  redrawwin(lw->w_status);
  wrefresh(lw->w_status);
}

// Shows an options dialogue
void
show_options(void)
{
  WINDOW * w_opt, * w_sub;
  MENU * m;
  const char * items[9];
  COORDS co;

  w_opt = create_win(0, 0, 0, 0, true, CP_WHITEBLACK);
  co = get_geometry(w_opt);
  set_winstr(w_opt, (co.x - (int) strlen("OPTIONS")) / 2, 1, A_BOLD,
             CP_WHITEBLACK, "OPTIONS");
  set_winstr(w_opt, (co.x - (int) strlen("(Allowed characters: 0-9, A-Z, "
             "space)")) / 2, 3, A_NORMAL, CP_WHITEBLACK, "(Allowed characters: "
             "0-9, A-Z, space)");
  w_sub = create_subwin(w_opt, 9, co.x - 2, 1, 8, false,
                         CP_WHITEBLACK);

  items[0] = "Move up";
  items[1] = "Move down";
  items[2] = "Move left";
  items[3] = "Move right";
  items[4] = "Shoot";
  items[5] = "Next weapon";
  items[6] = "Previous weapon";
  items[7] = "Open inventory";
  items[8] = "ACCEPT SETTINGS";
  m = create_menu(w_opt, w_sub, items, 9, CP_BLACKWHITE, CP_WHITEBLACK);

  for (;;)
  {
    int index, ch;
    WINDOW * w_prompt;

    redrawwin(w_sub);
    set_winstr(w_sub, 20, 0, A_BOLD, CP_WHITEBLACK, "%c",
               (isspace(cfg->up)) ? '_' : toupper(cfg->up));
    set_winstr(w_sub, 20, 1, A_BOLD, CP_WHITEBLACK, "%c",
               (isspace(cfg->down)) ? '_' : toupper(cfg->down));
    set_winstr(w_sub, 20, 2, A_BOLD, CP_WHITEBLACK, "%c",
               (isspace(cfg->left)) ? '_' : toupper(cfg->left));
    set_winstr(w_sub, 20, 3, A_BOLD, CP_WHITEBLACK, "%c",
               (isspace(cfg->right)) ? '_' : toupper(cfg->right));
    set_winstr(w_sub, 20, 4, A_BOLD, CP_WHITEBLACK, "%c",
               (isspace(cfg->use)) ? '_' : toupper(cfg->use));
    set_winstr(w_sub, 20, 5, A_BOLD, CP_WHITEBLACK, "%c",
               (isspace(cfg->nextw)) ? '_' : toupper(cfg->nextw));
    set_winstr(w_sub, 20, 6, A_BOLD, CP_WHITEBLACK, "%c",
               (isspace(cfg->prevw)) ? '_' : toupper(cfg->prevw));
    set_winstr(w_sub, 20, 7, A_BOLD, CP_WHITEBLACK, "%c",
               (isspace(cfg->inv)) ? '_' : toupper(cfg->inv));

    index = ctrl_menu(w_opt, m);
    if (index == 8)
    {
      break;
    }
    else
    {
      co = get_geometry(w_opt);
      w_prompt = create_win(3, co.x / 3, (co.x - co.x / 3) / 2,
                              (co.y - 3) / 2, true, CP_WHITEBLACK);
      co = get_geometry(w_prompt);
      set_winstr(w_prompt, (co.x - (int) strlen("Press a key.")) / 2, 1,
                 A_NORMAL, CP_WHITEBLACK, "Press a key.");
      set_inputmode(IM_KEYPRESS);
      ch = wgetch(w_prompt);
      rm_win(w_prompt);

      if (isalnum(ch) || (ch == ' '))
      {
        if ((ch != cfg->up) &&
            (ch != cfg->down) &&
            (ch != cfg->left) &&
            (ch != cfg->right) &&
            (ch != cfg->use) &&
            (ch != cfg->nextw) &&
            (ch != cfg->prevw) &&
            (ch != cfg->inv))
        {
          switch (index)
          {
            case 0:
              cfg->up = (char) ch;
              break;
            case 1:
              cfg->down = (char) ch;
              break;
            case 2:
              cfg->left = (char) ch;
              break;
            case 3:
              cfg->right = (char) ch;
              break;
            case 4:
              cfg->use = (char) ch;
              break;
            case 5:
              cfg->nextw = (char) ch;
              break;
            case 6:
              cfg->prevw = (char) ch;
              break;
            case 7:
              cfg->inv = (char) ch;
              break;
            default:
              break;
          }
        }
      }
    }
  }

  rm_menu(m);
  rm_win(w_sub);
  rm_win(w_opt);
  write_config();
}

// show_message with input control
bool
show_prompt(const char * msg, ...)
{
  MENU * m;
  WINDOW * w_msg, * w_menu;
  va_list args;
  const char * items[2];
  bool retval;
  int t_freeze;

  w_msg = create_win(2, 75, 0, CON_TERMY - 2, false, CP_BLACKWHITE);
  w_menu = create_win(2, 5, 75, CON_TERMY - 2, false, CP_BLACKWHITE);
  items[0] = " Yes ";
  items[1] = " No";
  m = create_menu(w_menu, w_menu, items, (int) (sizeof(items) / sizeof(char *)),
                  CP_WHITERED, CP_REDWHITE);

  va_start(args, msg);
  wmove(w_msg, 0, 0);
  vw_printw(w_msg, msg, args);
  va_end(args);
  wrefresh(w_msg);

  set_inputmode(IM_KEYPRESS);
  t_freeze = pause_game();

  while (true)
  {
    int index;

    index = ctrl_menu(w_menu, m);
    if (index == 0)
    {
      retval = true;
      break;
    }
    else
    {
      retval = false;
      break;
    }
  }

  resume_game(t_freeze);
  rm_menu(m);
  rm_win(w_menu);
  rm_win(w_msg);
  redrawwin(lw->w_status);
  wrefresh(lw->w_status);

  return retval;
}

// Shows a splashscreen at start
void
show_splashscreen(void)
{
  WINDOW * w_splash;

  w_splash = create_win(CON_TERMY, CON_TERMX, 0, 0, true, CP_WHITEBLACK);
  // Fancy splash screen (78 characters wide)
  mvwprintw(w_splash, (CON_TERMY - 7) / 2 + 0, (CON_TERMX - 78) / 2, "    #     #####   #####  ### ###     #####                                    ");
  mvwprintw(w_splash, (CON_TERMY - 7) / 2 + 1, (CON_TERMX - 78) / 2, "   # #   #     # #     #  #   #     #     #  ####  #    # #####    ##   ##### ");
  mvwprintw(w_splash, (CON_TERMY - 7) / 2 + 2, (CON_TERMX - 78) / 2, "  #   #  #       #        #   #     #       #    # ##  ## #    #  #  #    #   ");
  mvwprintw(w_splash, (CON_TERMY - 7) / 2 + 3, (CON_TERMX - 78) / 2, " #     #  #####  #        #   #     #       #    # # ## # #####  #    #   #   ");
  mvwprintw(w_splash, (CON_TERMY - 7) / 2 + 4, (CON_TERMX - 78) / 2, " #######       # #        #   #     #       #    # #    # #    # ######   #   ");
  mvwprintw(w_splash, (CON_TERMY - 7) / 2 + 5, (CON_TERMX - 78) / 2, " #     # #     # #     #  #   #     #     # #    # #    # #    # #    #   #   ");
  mvwprintw(w_splash, (CON_TERMY - 7) / 2 + 6, (CON_TERMX - 78) / 2, " #     #  #####   #####  ### ###     #####   ####  #    # #####  #    #   #   ");
  // ASCII art logo has 7 rows
  for (int i = 0; i <= 6; i++)
  {
    mvwchgat(w_splash, (CON_TERMY - 7) / 2 + i, 1, 78, A_BOLD, CP_WHITERED,
             NULL);
  }
  set_winstr(w_splash, (CON_TERMX - strlen("PRESS ENTER TO CONTINUE")) / 2,
             CON_TERMY - 2, A_BLINK, CP_REDBLACK, "PRESS ENTER TO CONTINUE");
  set_inputmode(IM_KEYPRESS);
  while (getch() != '\n');
  rm_win(w_splash);
}

// Shows the menu screen
void
show_startmenu(void)
{
  WINDOW * w_menu, * w_sub;
  COORDS co;
  const char * items[4];
  MENU * m;

  w_menu = create_win(CON_TERMY / 2, CON_TERMX / 2, CON_TERMX / 4,
                      CON_TERMY / 4, true, CP_WHITERED);
  co = get_geometry(w_menu);
  w_sub = create_subwin(w_menu, sizeof(items) / sizeof(items[0]), co.x - 4, 2,
          2, false, CP_WHITERED);
  items[0] = "Start Game";
  items[1] = "Show Highscore";
  items[2] = "Options";
  items[3] = "Quit";
  m = create_menu(w_menu, w_sub, items,
                  (int) (sizeof(items) / sizeof(char *)), CP_REDWHITE,
                  CP_WHITERED);
  set_winstr(w_menu, (co.x - (int) strlen("ASCII Combat ")
             - (int) strlen(INFO_VERSION)) / 2, 0, A_BOLD, CP_WHITERED,
             "ASCII Combat %s", INFO_VERSION);

  set_inputmode(IM_KEYPRESS);
  while (true)
  {
    int index;

    index = ctrl_menu(w_menu, m);
    if (index == 0)
    {
      run_game();
      redrawwin(w_menu);
      wrefresh(w_menu);
    }
    else if (index == 1)
    {
      show_highscore();
      redrawwin(w_menu);
      wrefresh(w_menu);
    }
    else if (index == 2)
    {
      show_options();
      redrawwin(w_menu);
      wrefresh(w_menu);
    }
    else if (index == 3)
    {
      rm_menu(m);
      rm_win(w_sub);
      rm_win(w_menu);
      break;
    }
  }
}

// Updates the status window at the bottom of the screen
void
init_status_window(PLAYER * p)
{
  // EUS
  set_winstr(lw->w_status, 5, 0, A_NORMAL, CP_WHITEBLACK, "PLACEHOLDER");
  // EXP
  set_winstr(lw->w_status, 5, 1, A_NORMAL, CP_WHITEBLACK, "PLACEHOLDER");
  // Health
  set_winstr(lw->w_status, lw->co_status.x / 3 + 9, 0, A_NORMAL, CP_GREENBLACK,
             "%d", p->hp);
  // Armour
  set_winstr(lw->w_status, lw->co_status.x / 3 + 9, 1, A_NORMAL, CP_GREENBLACK,
             "%d", p->armour);
  // Weapon
  set_winstr(lw->w_status, 2 * lw->co_status.x / 3 + 9, 0, A_NORMAL,
             CP_WHITEBLACK, "PLACEHOLDER");
  // Ammo
  set_winstr(lw->w_status, 2 * lw->co_status.x / 3 + 9, 1, A_NORMAL,
             CP_WHITEBLACK, "PLACEHOLDER");
}
