/* config.c: Configuration control.
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

#include "config.h"

CONFIG * cfg;

// Stores the config struct to a config file
void
ctrl_config(void)
{
  FILE * f_cfg;
  char * filename;
  struct passwd * passwd;

  passwd = getpwuid(getuid());
  filename = malloc(strlen(passwd->pw_dir) + strlen("/.accfg") + sizeof('\0'));
  strcpy(filename, passwd->pw_dir);
  strncat(filename, "/.accfg", 7);

  write_log(LOG_VERBOSE, "%s:\n\tWriting config to %s\n", __func__, filename);
  f_cfg = fopen(filename, "w");
  fwrite(cfg, sizeof(CONFIG), 1, f_cfg);
  fclose(f_cfg);
  f_cfg = NULL;
  _free(filename);
}

// Initialises the config struct
void
init_config(void)
{
  FILE * f_cfg;
  char * filename;
  struct passwd * passwd;

  cfg = malloc(sizeof(CONFIG));
  passwd = getpwuid(getuid());
  filename = malloc(strlen(passwd->pw_dir) + strlen("/.accfg") + sizeof('\0'));
  strcpy(filename, passwd->pw_dir);
  strncat(filename, "/.accfg", 7);

  f_cfg = fopen(filename, "r");
  if (f_cfg == NULL)
  {
    write_log(LOG_INFO, "%s:\n\tUnable to open file %s, creating new one\n",
              __func__, filename);
    f_cfg = fopen(filename, "w");
    write_log(LOG_VERBOSE, "\tCreated file %p\n\tWriting default config to %s\n",
              (void *) f_cfg, filename);

    cfg->up = 'w';
    cfg->down = 's';
    cfg->left = 'a';
    cfg->right = 'd';
    cfg->use = ' ';
    cfg->nextw = 'r';
    cfg->prevw = 'e';
    cfg->inv = 'f';

    fwrite(cfg, sizeof(CONFIG), 1, f_cfg);
  }
  else
  {
    write_log(LOG_VERBOSE, "%s:\n\tReading configuration from %s\n", __func__,
              filename);
    fread(cfg, sizeof(CONFIG), 1, f_cfg);
  }

  write_log(LOG_DEBUG, "\tcfg->up: %d\n\tcfg->down: %d"
            "\n\tcfg->left: %d\n\tcfg->right: %d\n\tcfg->use: %d"
            "\n\tcfg->nextw: %d\n\tcfg->prevw: %d\n\tcfg->inv: %d\n",
            cfg->up, cfg->down, cfg->left, cfg->right, cfg->use, cfg->nextw,
            cfg->prevw, cfg->inv);
  fclose(f_cfg);
  f_cfg = NULL;
  _free(filename);
}
