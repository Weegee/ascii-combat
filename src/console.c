/* console.c: Ncurses window management.
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

#include "console.h"

int g_fld[CON_FIELDMAXX + 1][CON_FIELDMAXY + 1];

/* Creates a ncurses menu using the specified windows, then sets the given
 * attributes */
MENU *
create_menu(WINDOW * w_menu, WINDOW * w_sub, const char ** items, int num,
            chtype cp_sel, chtype cp_unsel)
{
  MENU * m;
  ITEM ** li;

  li = calloc((size_t) num + 1, sizeof(ITEM *));
  for (int i = 0; i < num; i++)
  {
    li[i] = new_item(items[i], "");
  }
  li[num] = NULL; // Last menu item has to be NULL

  m = new_menu(li);
  set_menu_win(m, w_menu);
  set_menu_sub(m, w_sub);
  set_menu_fore(m, COLOR_PAIR(cp_sel));
  set_menu_back(m, COLOR_PAIR(cp_unsel));
  set_menu_mark(m, ""); // Disables prefix string for selected items
  post_menu(m);

  wrefresh(w_menu);
  write_log(LOG_VERBOSE, "%s:\n\tCreated menu %p\n", __func__, (void *) m);
  write_log(LOG_DEBUG, "\tw_menu: %p\n\tw_sub: %p\n\tli: %p\n\tnum: %d"
            "\n\tcp_sel: %d\n\tcp_unsel: %d\n", (void *) w_menu, (void *) w_sub,
            (void *) li, num, cp_sel, cp_unsel);
  return m;
}

// Creates a ncurses derived window with the specified properties
WINDOW *
create_subwin(WINDOW * w_parent, int rows, int cols, int x, int y, bool box,
              chtype cp)
{
  WINDOW * w_sub;

  w_sub = derwin(w_parent, rows, cols, y, x);
  write_log(LOG_VERBOSE, "%s:\n\tCreated sub window %p\n", __func__,
            (void *) w_sub);
  write_log(LOG_DEBUG, "\tw_parent: %p\n\trows: %d\n\tcols: %d\n\tx: %d"
            "\n\ty: %d\n\tbox: %d\n\tcp: %d\n", (void *) w_parent, rows, cols,
            x, y, box, cp);
  wbkgd(w_sub, COLOR_PAIR(cp));
  if (box)
  {
    box(w_sub, false, false); // Creates borders using the standard characters
    touchwin(w_parent);
    wrefresh(w_sub);
    return w_sub;
  }
  else
  {
    touchwin(w_parent);
    wrefresh(w_sub);
    return w_sub;
  }
}

// Creates a ncurses window with the specified properties
WINDOW *
create_win(int rows, int cols, int x, int y, bool box, chtype cp)
{
  WINDOW * w;

  w = newwin(rows, cols, y, x);
  write_log(LOG_VERBOSE, "%s:\n\tCreated window %p\n", __func__, (void *) w);
  write_log(LOG_DEBUG, "\trows: %d\n\tcols: %d\n\tx: %d\n\ty: %d\n\tbox: %d"
            "\n\tcp: %d\n", rows, cols, x, y, box, cp);
  wbkgd(w, COLOR_PAIR(cp));
  if (box)
  {
    box(w, false, false);
    wrefresh(w);
    return w;
  }
  else
  {
    wrefresh(w);
    return w;
  }
}

// Wrapper function for the menu driver
int
ctrl_menu(WINDOW * w, MENU * m)
{
  int key;

  while ((key = getch()) != '\n')
  {
    switch (key)
    {
      case KEY_DOWN:
        menu_driver(m, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        menu_driver(m, REQ_UP_ITEM);
        break;
      default:
        break;
    }
    wrefresh(w);
  }

  return item_index(current_item(m));
}

// Returns the window geometry of a window
COORDS
get_geometry(WINDOW * w)
{
  COORDS co;

  getmaxyx(w, co.y, co.x);
  return co;
}

// Initiates ncurses, shows a splash screen
void
init_console(void)
{
  COORDS co;

  initscr();
  co = get_geometry(stdscr);

  if (co.y < CON_TERMY || co.x < CON_TERMX)
  {
    endwin();
    printf("Your terminal is too small (%dx%d), the required size for "
           "ASCII Combat is %dx%d.\n"
           "Please adjust your terminal in order to play ASCII Combat.\n",
           co.x, co.y, CON_TERMX, CON_TERMY);
    exit(EXIT_FAILURE);
  }
  resize_term(CON_TERMY, CON_TERMX);
  clear();

  if (has_colors() == false)
  {
    endwin();
    printf("Your terminal doesn't support colours.\n");
    exit(EXIT_FAILURE);
  }
  start_color();
  init_pair(CP_WHITEBLACK, COLOR_WHITE, COLOR_BLACK);
  init_pair(CP_WHITERED, COLOR_WHITE, COLOR_RED);
  init_pair(CP_REDBLACK, COLOR_RED, COLOR_BLACK);
  init_pair(CP_YELLOWBLACK, COLOR_YELLOW, COLOR_BLACK);
  init_pair(CP_MAGENTABLACK, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(CP_REDWHITE, COLOR_RED, COLOR_WHITE);
  init_pair(CP_BLACKWHITE, COLOR_BLACK, COLOR_WHITE);
  init_pair(CP_GREENBLACK, COLOR_GREEN, COLOR_BLACK);
  init_pair(CP_WHITEBLUE, COLOR_WHITE, COLOR_BLUE);

  keypad(stdscr, true);
  cbreak();

  refresh();
  g_log = fopen("debug.log", "w");
  write_log(LOG_INFO, "%s:\n\tInitialised ncurses\n", __func__);
}

// Initialises the playing field by setting ENT_NOTHING on all fields
void
init_field(void)
{
  memset(g_fld, ENT_NOTHING, sizeof(g_fld));
  write_log(LOG_VERBOSE, "%s:\n\tInitialised the playing field\n", __func__);
}

// Initialises the game/status windows
WINDOWLIST *
init_windows(void)
{
  WINDOWLIST * lw;
  COORDS co;

  lw = malloc(sizeof(WINDOWLIST));

  lw->w_game = create_win(CON_TERMY - 2, CON_TERMX, 0, 0, true, CP_WHITEBLACK);
  lw->w_field = create_subwin(lw->w_game, CON_FIELDMAXY + 1, CON_FIELDMAXX + 1,
                              1, 1, false, CP_WHITEBLACK);
  lw->w_status = create_win(2, CON_TERMX, 0, CON_TERMY - 2, false,
                            CP_WHITEBLACK);
  co = get_geometry(lw->w_status);

  set_winstr(lw->w_status, 1, 0, A_BOLD, CP_WHITEBLACK, "EUS");
  set_winstr(lw->w_status, 1, 1, A_BOLD, CP_WHITEBLACK, "EXP");
  set_winstr(lw->w_status, co.x / 3 + 2, 0, A_BOLD, CP_WHITEBLACK, "HEALTH");
  set_winstr(lw->w_status, co.x / 3 + 2, 1, A_BOLD, CP_WHITEBLACK, "ARMOUR");
  set_winstr(lw->w_status, 2 * co.x / 3 + 2, 0, A_BOLD, CP_WHITEBLACK,
             "WEAPON");
  set_winstr(lw->w_status, 2 * co.x / 3 + 4, 1, A_BOLD, CP_WHITEBLACK, "AMMO");

  write_log(LOG_DEBUG, "%s:\n\tInitialised windows\n\tlw: %p\n\tw_game: %p"
            "\n\tw_field: %p\n\tw_status: %p\n", __func__, (void *) lw,
            (void *) lw->w_game, (void *) lw->w_field, (void *) lw->w_status);
  return lw;
}

// Destroys a ncurses menu
void
rm_menu(MENU * m)
{
  ITEM ** li;
  WINDOW * w_menu, * w_sub;
  int num;

  li = menu_items(m);
  w_menu = menu_win(m);
  w_sub = menu_sub(m);
  num = item_count(m);
  write_log(LOG_VERBOSE, "%s:\n\tRemoving menu %p\n", __func__, (void *) m);
  write_log(LOG_DEBUG, "\tli: %p\n\tw_menu: %p\n\tw_sub: %p\n\tnum: %d\n",
            (void *) m, (void *) li, (void *) w_menu, (void *) w_sub, num);

  unpost_menu(m);
  free_menu(m);
  m = NULL;
  for (int i = 0; i <= num; i++)
  {
    free_item(li[i]);
    li[i] = NULL;
  }
  _free(li);
  wrefresh(w_menu);
  wrefresh(w_sub);
}

// Destroys a ncurses window
void
rm_win(WINDOW * w)
{
  write_log(LOG_VERBOSE, "%s:\n\tRemoving window %p\n", __func__, (void *) w);
  wclear(w);
  wrefresh(w);
  delwin(w);
  w = NULL;
}

// Changes the ncurses user input mode
void
set_inputmode(int mode)
{
  switch (mode)
  {
    case IM_TEXTINPUT:
      // Shows the cursor and text while typing
      echo();
      curs_set(true);
      break;
    case IM_KEYPRESS:
      // Neither show the cursor nor the user input, make getch a blocking call
      noecho();
      curs_set(false);
      nodelay(stdscr, false);
      break;
    case IM_PLAYING:
      /* Same as IM_KEYPRESS, however getch is non-blocking here and returns
       * ERR when no key is pressed */
      noecho();
      curs_set(false);
      nodelay(stdscr, true);
      break;
    default:
      break;
  }
}

/* Prints a character in the given window at the specified position, then
 * changes its attributes */
void
set_winchar(WINDOW * w, int x, int y, attr_t a, short cp, char ch)
{
  mvwaddch(w, y, x, (chtype) ch);
  mvwchgat(w, y, x, 1, a, cp, NULL);
  wrefresh(w);
}

/* Prints a formatted string in the given window at the specified position,
 * then changes its attributes */
void
set_winstr(WINDOW * w, int x, int y, attr_t a, short cp, const char * str, ...)
{
  va_list args;
  int len;

  va_start(args, str);
  /* vsnprintf returns the length of str (number of characters without \0) if
   * there isn't enough space for printing the string, which is always the case
   * if we call vsnprintf with 0 as the buffer size */
  len = vsnprintf(NULL, 0, str, args);
  va_end(args);

  va_start(args, str);
  wmove(w, y, x);
  wrefresh(w);
  vw_printw(w, str, args);
  va_end(args);
  mvwchgat(w, y, x, len, a, cp, NULL);
  wrefresh(w);
}
