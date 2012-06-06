/* game.c: Main game events.
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

/* Controls all collisions between entities, starting with bullets, followed by
 * enemies and obstacles */
// TODO: This is way too nested
void
ctrl_collision(WINDOW * w_field, BULLETLIST * lb, ENEMYLIST * le,
               OBSTACLELIST * lo, PLAYER * p)
{
  BULLET * b;
  ENEMY * e;
  OBSTACLE * o;

  // Bullet collision check
  for (b = lb->head; b != NULL; b = b->next)
  {
    if (p->x == b->x && p->y == b->y)
    {
      write_log(LOG_INFO, "Player %p collided with bullet %p at (%d|%d)\n",
                (void *) p, (void *) b, p->x, p->y);
      rm_bullet(w_field, lb, b);
      set_player_dmg(w_field, p, BU_PLDAMAGE);
    }

    for (e = le->head; e != NULL; e = e->next)
    {
      if (b->x == e->x && b->y == e->y)
      {
        write_log(LOG_INFO, "Bullet %p collided with enemy %p at (%d|%d)\n",
                  (void *) b, (void *) e, b->x, b->y);
        rm_bullet(w_field, lb, b);
        set_enemy_dmg(w_field, e, BU_PLDAMAGE);

        if (e->hp == 0)
        {
          int x, y;

          switch (e->type)
          {
            case EN_REG:
              p->score += EN_REGSCORE;
              break;
            case EN_KAM:
              p->score += EN_KAMSCORE;
              break;
            default:
              break;
          }

          x = e->x;
          y = e->y;
          rm_enemy(w_field, le, e);
          set_winchar(w_field, x, y, A_BOLD, CP_YELLOWBLACK, '*');
        }
      }
    }

    for (o = lo->head; o != NULL; o = o->next)
    {
      if (b->x == o->x && b->y == o->y)
      {
        write_log(LOG_INFO, "Bullet %p collided with obstacle %p at (%d|%d)\n",
                  (void *) b, (void *) o, b->x, b->y);
        rm_bullet(w_field, lb, b);
        set_obstacle_dmg(w_field, o, BU_PLDAMAGE);

        if (o->hp == 0)
        {
          int x, y;

          switch (o->type)
          {
            case OB_REG:
              p->score += OB_REGSCORE;
              break;
            default:
              break;
          }

          x = o->x;
          y = o->y;
          rm_obstacle(w_field, lo, o);
          set_winchar(w_field, x, y, A_BOLD, CP_YELLOWBLACK, '*');
        }
      }
    }
  }

  // Enemy collision check
  for (e = le->head; e != NULL; e = e->next)
  {
    if (p->x == e->x && p->y == e->y)
    {
      int dmg;

      write_log(LOG_INFO, "Player %p collided with enemy %p at (%d|%d)\n",
                (void *) p, (void *) e, p->x, p->y);
      switch (e->type)
      {
        case EN_REG:
          dmg = EN_REGDAMAGE;
          break;
        case EN_KAM:
          dmg = EN_KAMDAMAGE;
          break;
        default:
          dmg = false;
          break;
      }
      rm_enemy(w_field, le, e);
      set_player_dmg(w_field, p, dmg);
    }

    for (o = lo->head; o != NULL; o = o->next)
    {
      if (e->x == o->x && e->y == o->y)
      {
        int x, y;

        x = e->x;
        y = e->y;
        write_log(LOG_INFO, "Enemy %p collided with obstacle %p at (%d|%d)\n",
                  (void *) e, (void *) o, x, y);
        rm_enemy(w_field, le, e);
        rm_obstacle(w_field, lo, o);
        set_winchar(w_field, x, y, A_BOLD, CP_YELLOWBLACK, '*');
      }
    }
  }

  // Obstacle collision check
  for (o = lo->head; o != NULL; o = o->next)
  {
    if (p->x == o->x && p->y == o->y)
    {
      int dmg;

      write_log(LOG_INFO, "Player %p collided with obstacle %p at (%d|%d)\n",
                (void *) p, (void *) o, p->x, p->y);
      switch (o->type)
      {
        case OB_REG:
          dmg = OB_REGDAMAGE;
          break;
        default:
          dmg = false;
          break;
      }
      rm_obstacle(w_field, lo, o);
      set_player_dmg(w_field, p, dmg);
    }
  }
}

/* Checks if the current score is higher than the lowest score in the highscore
 * file, then prints the highscore to the file */
SCORES *
ctrl_highscore(int p_score)
{
  char * filename;
  FILE * f_sc;
  SCORES * sc;
  struct passwd * passwd;

  passwd = getpwuid(getuid());
  sc = malloc(SCOREARRAYSIZE * sizeof(SCORES));
  /* The highscore has 10 entries, the scores array has 11. The current score
   * and player name are copied to the last index of the array, which then gets
   * sorted by qsort. If the player's score is high enough, it will be in the
   * first 10 entries of the scores array and thus be written to the highscore
   * file, which only contains the first 10 entries of the scores array. */
  filename = malloc(strlen(passwd->pw_dir) + strlen("/.acsc") + sizeof('\0'));
  for (int i = 0; i < SCOREARRAYSIZE; i++)
  {
    // Initialises the array with "empty" default values
    strcpy(sc[i].name, "-");
    sc[i].score = 0;
  }
  strcpy(sc[SCORESIZE].name, cfg->p_name);
  sc[SCORESIZE].score = p_score;

  strcpy(filename, passwd->pw_dir);
  strncat(filename, "/.acsc", 6);
  f_sc = fopen(filename, "r+");
  if (f_sc == NULL)
  {
    write_log(LOG_INFO, "Unable to open file %s, creating new one\n", filename);
    f_sc = fopen(filename, "w+");
    write_log(LOG_VERBOSE, "Created file %p\n", (void *) f_sc);

    qsort(sc, SCOREARRAYSIZE, sizeof(SCORES), cmp_scores);
    fwrite(sc, sizeof(SCORES), SCORESIZE, f_sc);
  }
  else
  {
    write_log(LOG_VERBOSE, "Reading scores from %s\n", filename);
    fread(sc, sizeof(SCORES), SCORESIZE, f_sc);
    for (int i = 0; i < SCOREARRAYSIZE; i++)
    {
      write_log(LOG_DEBUG, "sc[%d].name: %s; sc[%d].score: %d\n", i, sc[i].name,
                i, sc[i].score);
    }
    rewind(f_sc);
    qsort(sc, SCOREARRAYSIZE, sizeof(SCORES), cmp_scores);
    fwrite(sc, sizeof(SCORES), SCORESIZE, f_sc);
  }
  write_log(LOG_VERBOSE, "Scores written to file %p at %s\n", (void *) f_sc,
            filename);

  fclose(f_sc);
  free(filename);
  return sc;
}

// Initiates the game, shows a start screen
void
init_game()
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
loop_game(WINDOWLIST * lw, BULLETLIST * lb, ENEMYLIST * le, OBSTACLELIST * lo,
          PLAYER * p, TIMER * t)
{
  struct timeval ct;
  long msec_elapsed;
  int retval = 1;

  gettimeofday(&ct, NULL);
  msec_elapsed = (long) (((ct.tv_sec - t->start) * 1000) + (ct.tv_usec / 1000));
  if (msec_elapsed % 100 == 0 && msec_elapsed != t->msec_elapsed)
  {
    t->msec_elapsed = msec_elapsed;
    t->sec_elapsed = (int) (t->msec_elapsed / 1000);
    ctrl_enemy_kamikaze(lw->w_field, le, p);
    ctrl_collision(lw->w_field, lb, le, lo, p);
    update_status_windows(lw, p);

    if (msec_elapsed % 500 == 0)
    {
      ctrl_bullets(lw->w_field, lb);
      ctrl_collision(lw->w_field, lb, le, lo, p);

      if (msec_elapsed % 1000 == 0)
      {
        ctrl_timer(lw->w_game, t);
        ctrl_obstacles(lw->w_field, lo, t->sec_elapsed);
        ctrl_collision(lw->w_field, lb, le, lo, p);
        ctrl_enemy_spawn(lw->w_field, le, t->sec_elapsed);
        ctrl_collision(lw->w_field, lb, le, lo, p);
      }
    }
  }

  ctrl_player(lw->w_game, lw->w_field, lb, p, t);
  ctrl_collision(lw->w_field, lb, le, lo, p);

  if (p->hp == 0)
  {
    write_log(LOG_INFO, "Player is dead, stopping game ...\n");
    retval = 0;
  }
  if (p->quit == true)
  {
    write_log(LOG_INFO, "Player wants to quit, stopping ...\n");
    retval = 0;
  }
  return retval;
}

// Removes all entities, lists and windows in order to quit the game
void
quit_game(WINDOWLIST * lw, BULLETLIST * lb, ENEMYLIST * le, OBSTACLELIST * lo,
          PLAYER * p, TIMER * t)
{
  rm_win(lw->w_field);
  rm_win(lw->w_game);
  rm_win(lw->w_ammo);
  rm_win(lw->w_hp);
  rm_win(lw->w_score);
  rm_obstaclelist(lo);
  rm_bulletlist(lb);
  rm_enemylist(le);

  show_highscore(ctrl_highscore(p->score));

  free(t);
  free(p);
  free(lw);
  free(cfg);
  endwin();
  write_log(LOG_INFO, "Quitting game, goodbye!\n");
  fclose(g_log);
}

// Shows an options dialogue
void
show_options()
{
  WINDOW * w_opt, * w_sub;
  FIELD * fld[10];
  FORM * f;
  int ch;
  COORDS co;
  char buf[P_MAXNAMELEN];

  // 10 = number of config values
  for (int i = 0; i < 10; i++)
  {
    write_log(LOG_DEBUG, "%d\n", i);
    if (i == 0)
    {
      fld[0] = new_field(1, P_MAXNAMELEN - 1, 0, 20, 0, 0);
    }
    else if (i == 9)
    {
      fld[i] = NULL;
    }
    else
    {
      fld[i] = new_field(1, 1, i, 20, 0, 0);
    }

    if (i != 9)
    {
      set_field_back(fld[i], A_BOLD | COLOR_PAIR(CP_WHITEBLACK));
      set_field_fore(fld[i], A_BOLD | COLOR_PAIR(CP_WHITEBLACK));
      field_opts_off(fld[i], O_AUTOSKIP);
      field_opts_on(fld[i], O_BLANK);
    }
  }

  set_field_buffer(fld[0], 0, cfg->p_name);
  set_field_buffer(fld[1], 0, &(cfg->up));
  set_field_buffer(fld[2], 0, &(cfg->down));
  set_field_buffer(fld[3], 0, &(cfg->left));
  set_field_buffer(fld[4], 0, &(cfg->right));
  set_field_buffer(fld[5], 0, &(cfg->use));
  set_field_buffer(fld[6], 0, &(cfg->nextw));
  set_field_buffer(fld[7], 0, &(cfg->prevw));
  set_field_buffer(fld[8], 0, &(cfg->inv));

  w_opt = create_win(0, 0, 0, 0, true, CP_WHITEBLACK);
  co = get_geometry(w_opt);
  set_winstr(w_opt, (co.x - (int) strlen("OPTIONS")) / 2, 1, A_BOLD,
             CP_WHITEBLACK, "OPTIONS");
  set_winstr(w_opt, (co.x - (int) strlen("(Allowed characters: 0-9, A-Z, space "
             "for action keys)")) / 2, 3, A_NORMAL, CP_WHITEBLACK, "(Allowed "
             "characters: 0-9, A-Z, space for action keys)");
  set_winstr(w_opt, (co.x - (int) strlen("PRESS ENTER TO ACCEPT")) / 2,
             co.y - 2, A_NORMAL, CP_REDBLACK, "PRESS ENTER TO ACCEPT");
  // co.y - 9: 6 lines from the top, 3 lines from the bottom
  w_sub = create_subwin(w_opt, co.y - 9, co.x - 2, 1, 5, false,
                         CP_WHITEBLACK);
  f = create_form(w_opt, w_sub, fld);

  set_winstr(w_sub, 0, 0, A_NORMAL, CP_WHITEBLACK, "Name");
  set_winstr(w_sub, 0, 1, A_NORMAL, CP_WHITEBLACK, "Move up");
  set_winstr(w_sub, 0, 2, A_NORMAL, CP_WHITEBLACK, "Move down");
  set_winstr(w_sub, 0, 3, A_NORMAL, CP_WHITEBLACK, "Move left");
  set_winstr(w_sub, 0, 4, A_NORMAL, CP_WHITEBLACK, "Move right");
  set_winstr(w_sub, 0, 5, A_NORMAL, CP_WHITEBLACK, "Shoot/Use item");
  set_winstr(w_sub, 0, 6, A_NORMAL, CP_WHITEBLACK, "Next weapon");
  set_winstr(w_sub, 0, 7, A_NORMAL, CP_WHITEBLACK, "Previous weapon");
  set_winstr(w_sub, 0, 8, A_NORMAL, CP_WHITEBLACK, "Open inventory");

  // Form driver, controls user input
  keypad(w_opt, true);
  while ((ch = wgetch(w_opt)) != '\n')
  {
    char buf_keys[8];

    for (int i = 1; i < 9; i++)
    {
      buf_keys[i - 1] = *field_buffer(fld[i], 0);
    }

    switch (ch)
    {
      case KEY_LEFT:
        form_driver(f, REQ_PREV_CHAR);
        break;
      case KEY_RIGHT:
        form_driver(f, REQ_NEXT_CHAR);
        break;
      case KEY_BACKSPACE:
        form_driver(f, REQ_DEL_PREV);
        break;
      case 127: // VTEs use 127 as their backspace key
        form_driver(f, REQ_DEL_PREV);
        break;
      case 330: // DEL key
        form_driver(f, REQ_DEL_CHAR);
        break;
      case KEY_DOWN:
        set_inputmode(IM_TEXTINPUT);
        form_driver(f, REQ_NEXT_FIELD);
        form_driver(f, REQ_END_LINE);
        break;
      case KEY_UP:
        set_inputmode(IM_TEXTINPUT);
        form_driver(f, REQ_PREV_FIELD);
        form_driver(f, REQ_END_LINE);
        break;
      default:
        if (current_field(f) == fld[0]) // Name field
        {
          if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')
              || (ch >= '0' && ch <= '9'))
          {
            set_inputmode(IM_TEXTINPUT);
            form_driver(f, ch);
            form_driver(f, REQ_VALIDATION);
          }
          break;
        }
        else
        {
          // Prevent setting the same key for different actions
          bool key_set;

          key_set = false;
          for (int i = 0; i < 8; i++)
          {
            if (ch == buf_keys[i])
            {
              key_set = true;
            }
          }
          if (key_set == false)
          {
            if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')
                || (ch >= '0' && ch <= '9') || (ch == ' '))
            {
              set_inputmode(IM_TEXTINPUT);
              form_driver(f, ch);
              form_driver(f, REQ_VALIDATION);
            }
            break;
          }
        }
    }
  }

  /* TODO:
   * - Write the cfg struct to the config file (ctrl_config) */

  // Change player name
  strcpy(buf, field_buffer(fld[0], 0));
  if (buf[0] != ' ')
  {
    for (int i = 0; i < P_MAXNAMELEN; i++)
    {
      cfg->p_name[i] = ' ';
    }
    strtok(buf, " ");
    strcpy(cfg->p_name, buf);
  }
  write_log(LOG_DEBUG, "cfg->p_name: %s; buf: %s;\n", cfg->p_name, buf);

  // Store all action keys in the config struct
  cfg->up = *field_buffer(fld[1], 0);
  cfg->down = *field_buffer(fld[2], 0);
  cfg->left = *field_buffer(fld[3], 0);
  cfg->right = *field_buffer(fld[4], 0);
  cfg->use = *field_buffer(fld[5], 0);
  cfg->nextw = *field_buffer(fld[6], 0);
  cfg->prevw = *field_buffer(fld[7], 0);
  cfg->inv = *field_buffer(fld[8], 0);

  rm_form(f);
  for (int i = 0; i < (int) (sizeof(fld) / sizeof(fld[0])); i++)
  {
    free_field(fld[i]);
  }
  set_inputmode(IM_KEYPRESS);
  rm_win(w_sub);
  rm_win(w_opt);
  ctrl_config();
}
// Displays the current highscore
void
show_highscore(SCORES * sc)
{
  WINDOW * w_scores;
  COORDS co;

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
  free(sc);
}

// Shows the menu screen
void
show_startmenu()
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
      break;
    }
    else if (index == 1)
    {
      show_highscore(ctrl_highscore(0));
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
      endwin();
      exit(EXIT_SUCCESS);
    }
  }
}

// Updates the status windows at the bottom of the screen
void
update_status_windows(WINDOWLIST * lw, PLAYER * p)
{
  COORDS co;

  co = get_geometry(lw->w_ammo); // All status windows have the same geometry
  wmove(lw->w_ammo, 1, 0);
  wdeleteln(lw->w_ammo);
  wmove(lw->w_hp, 1, 0);
  wdeleteln(lw->w_hp);
  wmove(lw->w_score, 1, 0);
  wdeleteln(lw->w_score);

  set_winstr(lw->w_ammo, (co.x - get_intlen(p->ammo)) / 2, 1, A_NORMAL,
             CP_WHITEBLACK, "%d", p->ammo);
  set_winstr(lw->w_hp, (co.x - get_intlen(p->hp)) / 2, 1, A_NORMAL,
             CP_WHITEBLACK, "%d", p->hp);
  set_winstr(lw->w_score, (co.x - get_intlen(p->score)) / 2, 1, A_NORMAL,
             CP_WHITEBLACK, "%d", p->score);
}
