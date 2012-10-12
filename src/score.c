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
  _free(filename);
  _free(sc);
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
  _free(filename);
  _free(sc);
}
