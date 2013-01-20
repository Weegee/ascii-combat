/* score.c: Highscore control.
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

#include "score.h"

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
ctrl_highscore(int p_exp, int sec_elapsed)
{
  int p_score;
  char * filepath, p_name[P_MAXNAMELEN];
  FILE * f_sc;
  SCORES * sc;
  WINDOW * w_name;

  /* Allocate 11 (SCOREARRAYSIZE) elements instead of 10 (SCORESIZE) since we'll
   * write in the 11. element of the score array */
  sc = calloc(SCOREARRAYSIZE, sizeof(SCORES));
  filepath = get_highscore_path();
  write_log(LOG_DEBUG, "%s:\n\tfilepath: %s\n", __func__, filepath);
  p_score = p_exp + sec_elapsed / 2;
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

  f_sc = fopen(filepath, "r+");
  if (f_sc == NULL)
  {
    write_log(LOG_INFO, "%s:\n\tUnable to open file %s, creating new one\n",
              __func__, filepath);
    f_sc = fopen(filepath, "w+");
    write_log(LOG_VERBOSE, "\tCreated file %p\n", (void *) f_sc);

    qsort(sc, SCOREARRAYSIZE, sizeof(SCORES), cmp_scores);
    fwrite(sc, sizeof(SCORES), SCORESIZE, f_sc);
  }
  else
  {
    write_log(LOG_VERBOSE, "%s:\n\tReading scores from %s\n", __func__,
              filepath);
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
            filepath);
  fclose(f_sc);
  f_sc = NULL;
  _free(filepath);
  _free(sc);
}

// Returns the path to the highscore file
char *
get_highscore_path(void)
{
  char * filepath;

  if (getenv("XDG_DATA_HOME") != NULL)
  {
    write_log(LOG_VERBOSE, "%s:\n\tXDG_DATA_HOME is set\n", __func__);
    filepath = malloc(strlen(getenv("XDG_DATA_HOME"))
                      + strlen("/ac-scores.dat") + sizeof('\0'));
    strncpy(filepath, getenv("XDG_DATA_HOME"),
            strlen(getenv("XDG_DATA_HOME")) + sizeof('\0'));
    strncat(filepath, "/ac-scores.dat", 14);
  }
  else
  {
    char * dirpath;
    struct passwd * passwd;

    write_log(LOG_VERBOSE, "%s:\n\tXDG_DATA_HOME isn't set, falling back to "
              "$HOME/.local/share\n", __func__);
    passwd = getpwuid(getuid());
    dirpath = malloc(strlen(passwd->pw_dir) + strlen("/.local/share")
                     + sizeof('\0'));
    strncpy(dirpath, passwd->pw_dir, strlen(passwd->pw_dir) + sizeof('\0'));
    strncat(dirpath, "/.local", 7);
    if (mkdir(dirpath, 0755) == -1)
    {
      write_log(LOG_INFO, "%s:\n\tDirectory %s already exists\n", __func__,
                dirpath);
    }
    else
    {
      write_log(LOG_INFO, "%s:\n\tCreating directory %s\n", __func__, dirpath);
    }
    strncat(dirpath, "/share", 6);
    if (mkdir(dirpath, 0755) == -1)
    {
      write_log(LOG_INFO, "%s:\n\tDirectory %s already exists\n", __func__,
                dirpath);
    }
    else
    {
      write_log(LOG_INFO, "%s:\n\tCreating directory %s\n", __func__, dirpath);
    }
    filepath = malloc(strlen(dirpath) + strlen("/ac-scores.dat")
                      + sizeof('\0'));
    strncpy(filepath, dirpath, strlen(dirpath) + sizeof('\0'));
    strncat(filepath, "/ac-scores.dat", 14);
    _free(dirpath);
  }

  write_log(LOG_DEBUG, "\tfilepath: %s\n", filepath);
  return filepath;
}

// Displays the current highscore
void
show_highscore(void)
{
  FILE * f_sc;
  char * filepath;

  filepath = get_highscore_path();
  f_sc = fopen(filepath, "r");
  if (f_sc == NULL)
  {
    write_log(LOG_INFO, "%s:\n\tUnable to open file %s\n", __func__, filepath);
  }
  else
  {
    WINDOW * w_scores;
    COORDS co;
    SCORES * sc;

    sc = calloc(SCORESIZE, sizeof(SCORES));
    write_log(LOG_VERBOSE, "%s:\n\tReading scores from %s\n", __func__,
              filepath);
    fread(sc, sizeof(SCORES), SCORESIZE, f_sc);
    fclose(f_sc);

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
    _free(sc);
  }

  f_sc = NULL;
  _free(filepath);
}
