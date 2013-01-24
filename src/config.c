/* config.c: Configuration control.
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

#include "config.h"

CONFIG * cfg;

// Returns the path to the configuration file
char *
get_config_path(void)
{
  char * filepath;

  if (getenv("XDG_CONFIG_HOME") != NULL)
  {
    write_log(LOG_VERBOSE, "%s:\n\tXDG_CONFIG is set\n", __func__);
    filepath = malloc(strlen(getenv("XDG_CONFIG_HOME"))
                      + strlen("/ascii-combat.conf") + sizeof('\0'));
    strncpy(filepath, getenv("XDG_CONFIG_HOME"),
            strlen(getenv("XDG_CONFIG_HOME")) + sizeof('\0'));
    strncat(filepath, "/ascii-combat.conf", 18);
  }
  else
  {
    char * dirpath;
    struct passwd * passwd;

    passwd = getpwuid(getuid());
    dirpath = malloc(strlen(passwd->pw_dir) + strlen("/.config")
                     + sizeof('\0'));
    strncpy(dirpath, passwd->pw_dir, strlen(passwd->pw_dir) + sizeof('\0'));
    strncat(dirpath, "/.config", 8);
    if (mkdir(dirpath, 0755) == -1)
    {
      write_log(LOG_INFO, "%s:\n\tDirectory %s already exists\n", __func__,
                dirpath);
    }
    else
    {
      write_log(LOG_INFO, "%s:\n\tCreating directory %s\n", __func__, dirpath);
    }
    filepath = malloc(strlen(dirpath) + strlen("/ascii-combat.conf")
                      + sizeof('\0'));
    strncpy(filepath, dirpath, strlen(dirpath) + sizeof('\0'));
    strncat(filepath, "/ascii-combat.conf", 18);
    _free(dirpath);
  }

  write_log(LOG_DEBUG, "\tfilepath: %s\n", filepath);
  return filepath;
}

// Initialises the config struct by reading the configuration file
void
read_config(void)
{
  FILE * f_cfg;
  char * filepath;

  cfg = malloc(sizeof(CONFIG));
  filepath = get_config_path();

  f_cfg = fopen(filepath, "r");
  if (f_cfg == NULL)
  {
    write_log(LOG_INFO, "%s:\n\tUnable to open file %s, creating new one\n",
              __func__, filepath);
    write_log(LOG_VERBOSE, "\tWriting default config to %s\n", filepath);

    cfg->up = 'w';
    cfg->down = 's';
    cfg->left = 'a';
    cfg->right = 'd';
    cfg->use = ' ';
    cfg->nextw = 'r';
    cfg->prevw = 'e';
    cfg->inv = 'f';

    write_config();
  }
  else
  {
    write_log(LOG_VERBOSE, "%s:\n\tReading configuration from %s\n", __func__,
              filepath);
    fread(cfg, sizeof(CONFIG), 1, f_cfg);
    fclose(f_cfg);
    f_cfg = NULL;
  }

  write_log(LOG_DEBUG, "\tcfg->up: %d\n\tcfg->down: %d"
            "\n\tcfg->left: %d\n\tcfg->right: %d\n\tcfg->use: %d"
            "\n\tcfg->nextw: %d\n\tcfg->prevw: %d\n\tcfg->inv: %d\n",
            cfg->up, cfg->down, cfg->left, cfg->right, cfg->use, cfg->nextw,
            cfg->prevw, cfg->inv);
  _free(filepath);
}

// Writes the config struct to the config file
void
write_config(void)
{
  FILE * f_cfg;
  char * filepath;

  filepath = get_config_path();
  write_log(LOG_VERBOSE, "%s:\n\tWriting config to %s\n", __func__, filepath);
  f_cfg = fopen(filepath, "w");
  fwrite(cfg, sizeof(CONFIG), 1, f_cfg);
  fclose(f_cfg);
  f_cfg = NULL;
  _free(filepath);
}
