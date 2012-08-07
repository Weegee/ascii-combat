/* game.c: Main game events and game control.
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

// Compare function for qsort
int
cmp_scores(const void * sc1, const void * sc2)
{
  const SCORES * sc_cmp1 = (const SCORES *) sc1;
  const SCORES * sc_cmp2 = (const SCORES *) sc2;

  if (sc_cmp1->score > sc_cmp2->score)
  {
    return -1;
  }
  else if (sc_cmp1->score < sc_cmp2->score)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/* Checks if the current score is higher than the lowest score in the highscore
 * file, then prints the highscore to the file */
void
ctrl_highscore(int p_exp)
{
  int p_score;
  char * filename, p_name[P_MAXNAMELEN];
  FILE * f_sc;
  SCORES * sc;
  struct passwd * passwd;
  WINDOW * w_name;

  passwd = getpwuid(getuid());
  sc = calloc(SCOREARRAYSIZE, sizeof(SCORES));
  filename = malloc(strlen(passwd->pw_dir) + strlen("/.acsc") + sizeof('\0'));
  strcpy(filename, passwd->pw_dir);
  strncat(filename, "/.acsc", 6);

  p_score = p_exp + t->sec_elapsed / 2;
  /* The highscore has 10 entries, the scores array has 11. The current score
   * and player name are copied to the last index of the array, which then gets
   * sorted by qsort. If the player's score is high enough, it will be in the
   * first 10 entries of the scores array and thus be written to the highscore
   * file, which only contains the first 10 entries of the scores array. */
  for (int i = 0; i < SCOREARRAYSIZE; i++)
  {
    // Initialises the array with "empty" default values
    strcpy(sc[i].name, "-");
    sc[i].score = 0;
  }

  // Ask for the player name
  set_inputmode(IM_TEXTINPUT);
  w_name = create_win(0, 0, 0, 0, false, CP_WHITEBLACK);
  set_winstr(w_name, 0, CON_TERMY - 1, A_NORMAL, CP_WHITEBLACK,
             "Enter your name: ");
  mvwgetnstr(w_name, CON_TERMY - 1, 17, p_name, P_MAXNAMELEN);
  rm_win(w_name);

  if (!strcmp(p_name, ""))
  {
    strcpy(p_name, "Unknown");
  }
  strcpy(sc[SCORESIZE].name, p_name);
  sc[SCORESIZE].score = p_score;

  f_sc = fopen(filename, "r+");
  if (f_sc == NULL)
  {
    write_log(LOG_INFO, "%s:\n\tUnable to open file %s, creating new one\n",
              __func__, filename);
    f_sc = fopen(filename, "w+");
    write_log(LOG_VERBOSE, "\tCreated file %p\n", (void *) f_sc);

    qsort(sc, SCOREARRAYSIZE, sizeof(SCORES), cmp_scores);
    fwrite(sc, sizeof(SCORES), SCORESIZE, f_sc);
  }
  else
  {
    write_log(LOG_VERBOSE, "%s:\n\tReading scores from %s\n", __func__,
              filename);
    fread(sc, sizeof(SCORES), SCORESIZE, f_sc);
    for (int i = 0; i < SCOREARRAYSIZE; i++)
    {
      write_log(LOG_DEBUG, "\tsc[%d].name: %s - sc[%d].score: %d\n", i,
                sc[i].name, i, sc[i].score);
    }

    qsort(sc, SCOREARRAYSIZE, sizeof(SCORES), cmp_scores);
    rewind(f_sc);
    fwrite(sc, sizeof(SCORES), SCORESIZE, f_sc);
  }

  write_log(LOG_VERBOSE, "\tScores written to file %p (%s)\n", (void *) f_sc,
            filename);
  fclose(f_sc);
  f_sc = NULL;
  free(filename);
  filename = NULL;
  free(sc);
  sc = NULL;
}

// Clears all essential data (config, log file), ends ncurses
void
exit_game(void)
{
  endwin();
  free(cfg);
  cfg = NULL;
  write_log(LOG_INFO, "%s:\n\tQuitting game, goodbye!\n", __func__);
  fclose(g_log);
  g_log = NULL;
}

// Shows the start screen, then switches to the main menu
void
init_game(void)
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
  show_startmenu();
}

// Main game loop, calls all control functions after a certain time
int
loop_game(WINDOWLIST * lw, PLAYER * p)
{
  struct timeval ct;
  long msec_elapsed;
  int retval;

  retval = 1;
  gettimeofday(&ct, NULL);
  msec_elapsed = (long) (((ct.tv_sec - t->start) * 1000) + (ct.tv_usec / 1000));
  if (msec_elapsed % 100 == 0 && msec_elapsed != t->msec_elapsed)
  {
    t->msec_elapsed = msec_elapsed;
    t->sec_elapsed = (int) (t->msec_elapsed / 1000);
    update_status_window(lw->w_status, p);

    if (msec_elapsed % 1000 == 0)
    {
      ctrl_timer(lw->w_game);

      if (msec_elapsed % 10000 == 0)
      {
        p->stage++;
        set_winstr(lw->w_game, 10, 0, A_NORMAL, CP_WHITEBLACK, "S: %02d",
                   p->stage);
      }
    }
  }

  ctrl_player(lw, p);
  if (p->quit == true)
  {
    write_log(LOG_INFO, "%s:\n\tPlayer wants to quit, stopping ...\n", __func__);
    retval = 0;
  }

  return retval;
}

// Removes all entities, lists and windows
void
quit_game(WINDOWLIST * lw, PLAYER * p)
{
  rm_win(lw->w_field);
  rm_win(lw->w_game);
  rm_win(lw->w_status);

  ctrl_highscore(p->exp);
  show_highscore();

  free(p);
  p = NULL;
  free(lw);
  lw = NULL;
  free(t);
  t = NULL;
}

// Initialises the playing field and all entities
void
run_game(void)
{
  PLAYER * p;
  WINDOWLIST * lw;

  init_field();
  lw = init_windows();
  p = create_player(lw);
  p->stage = 1;
  set_winstr(lw->w_game, 10, 0, A_NORMAL, CP_WHITEBLACK, "S: %02d", p->stage);
  init_timer(lw->w_game);
  while (loop_game(lw, p));
  quit_game(lw, p);
}

// Displays the current highscore
void
show_highscore(void)
{
  WINDOW * w_scores;
  COORDS co;
  SCORES * sc;
  FILE * f_sc;
  char * filename;
  struct passwd * passwd;

  passwd = getpwuid(getuid());
  filename = malloc(strlen(passwd->pw_dir) + strlen("/.acsc") + sizeof('\0'));
  strcpy(filename, passwd->pw_dir);
  strncat(filename, "/.acsc", 6);
  sc = calloc(SCORESIZE, sizeof(SCORES));

  f_sc = fopen(filename, "r");
  if (f_sc == NULL)
  {
    write_log(LOG_INFO, "%s:\n\tUnable to open file %s\n", __func__, filename);
    // Fill the array with empty values, ignore the file error
    for (int i = 0; i < SCORESIZE; i++)
    {
      strcpy(sc[i].name, "-");
      sc[i].score = 0;
    }
  }
  else
  {
    write_log(LOG_VERBOSE, "%s:\n\tReading scores from %s\n", __func__,
              filename);
    fread(sc, sizeof(SCORES), SCORESIZE, f_sc);
    fclose(f_sc);
  }

  w_scores = create_win(CON_TERMY, CON_TERMX, 0, 0, 1, CP_WHITEBLACK);
  co = get_geometry(w_scores);
  set_winstr(w_scores, (co.x - (int) strlen("HIGHSCORE")) / 2, 1, A_BOLD,
             CP_WHITEBLACK, "HIGHSCORE");
  for (int i = 0; i < SCORESIZE; i++)
  {
    /* Start from the sixth line, looks better than from the third; also show
     * the scores right-aligned */
    set_winstr(w_scores, 1, 6 + i, A_NORMAL, CP_WHITEBLACK, sc[i].name);
    set_winstr(w_scores, co.x - get_intlen(sc[i].score) - 1, 6 + i, A_NORMAL,
               CP_WHITEBLACK, "%d", sc[i].score);
  }
  set_winstr(w_scores, (co.x - (int) strlen("PRESS ENTER TO CONTINUE")) / 2,
             co.y - 2, A_BLINK, CP_REDBLACK, "PRESS ENTER TO CONTINUE");

  set_inputmode(IM_KEYPRESS);
  while (getch() != '\n');
  rm_win(w_scores);
  f_sc = NULL;
  free(filename);
  filename = NULL;
  free(sc);
  sc = NULL;
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

      if ((ch >= 'a' && ch <= 'z') ||
          (ch >= '0' && ch <= '9') ||
          (ch == ' '))
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
  ctrl_config();
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
  return retval;
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
update_status_window(WINDOW * w_status, PLAYER * p)
{
  COORDS co;
  short colour;

  co = get_geometry(w_status);

  /* Prevent flickering by only calling wrefresh once (that's why we don't use
   * set_winstr here) */
  werase(w_status);
  mvwprintw(w_status, 0, 1, "EUS PLACEHOLDER");
  mvwchgat(w_status, 0, 1, 3, A_BOLD, CP_WHITEBLACK, NULL);
  mvwprintw(w_status, 1, 1, "EXP PLACEHOLDER");
  mvwchgat(w_status, 1, 1, 3, A_BOLD, CP_WHITEBLACK, NULL);

  mvwprintw(w_status, 0, co.x / 3 + 2, "HEALTH %d", p->hp);
  mvwchgat(w_status, 0, co.x / 3 + 2, 6, A_BOLD, CP_WHITEBLACK, NULL);
  if (p->hp <= 66 && p->hp > 33)
  {
    colour = CP_YELLOWBLACK;
  }
  else if (p->hp <= 33)
  {
    colour = CP_REDBLACK;
  }
  else
  {
    colour = CP_GREENBLACK;
  }
  mvwchgat(w_status, 0, co.x / 3 + 9, get_intlen(p->hp), A_NORMAL, colour,
           NULL);
  mvwprintw(w_status, 1, co.x / 3 + 2, "ARMOUR %d", p->armour);
  mvwchgat(w_status, 1, co.x / 3 + 2, 6, A_BOLD, CP_WHITEBLACK, NULL);
  if (p->armour <= 66 && p->armour > 33)
  {
    colour = CP_YELLOWBLACK;
  }
  else if (p->armour <= 33)
  {
    colour = CP_REDBLACK;
  }
  else
  {
    colour = CP_GREENBLACK;
  }
  mvwchgat(w_status, 1, co.x / 3 + 9, get_intlen(p->armour), A_NORMAL,
           colour, NULL);

  mvwprintw(w_status, 0, 2 * co.x / 3 + 2, "WEAPON PLACEHOLDER");
  mvwchgat(w_status, 0, 2 * co.x / 3 + 2, 6, A_BOLD, CP_WHITEBLACK, NULL);
  mvwprintw(w_status, 1, 2 * co.x / 3 + 4, "AMMO PLACEHOLDER");
  mvwchgat(w_status, 1, 2 * co.x / 3 + 4, 4, A_BOLD, CP_WHITEBLACK, NULL);
  wrefresh(w_status);
}
