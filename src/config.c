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

// Writes the default values in the config struct
void
init_config(void)
{
  cfg->up = 'w';
  cfg->down = 's';
  cfg->left = 'a';
  cfg->right = 'd';
  cfg->use = ' ';
  cfg->next = 'r';
  cfg->prev = 'e';
  cfg->inv = 'f';
}

// Parses the config file
void
parse_config(FILE * f_cfg)
{
  char linebuffer[50];

  while (fgets(linebuffer, 50, f_cfg) != NULL)
  {
    char * key, * value;

    if (linebuffer[0] == '#')
    {
      continue;
    }

    value = split_str(linebuffer, '=');
    if (value == NULL)
    {
      write_log(LOG_INFO, "%s:\n\tInvalid syntax: %s\n", __func__, linebuffer);
      continue;
    }

    key = trim_str(linebuffer, ' ');
    value = trim_str(value, '\n');
    value = trim_str(value, ' ');
    value = trim_str(value, '"');

    if (!isalnum(value[0]) && value[0] != ' ')
    {
      write_log(LOG_INFO, "%s:\n\tIllegal value '%s' for key '%s'\n",
                __func__, value, key);
      continue;
    }

    if (!strcmp(key, "up"))
    {
      cfg->up = (unsigned char) tolower(value[0]);
    }
    else if (!strcmp(key, "down"))
    {
      cfg->down = (unsigned char) tolower(value[0]);
    }
    else if (!strcmp(key, "left"))
    {
      cfg->left = (unsigned char) tolower(value[0]);
    }
    else if (!strcmp(key, "right"))
    {
      cfg->right = (unsigned char) tolower(value[0]);
    }
    else if (!strcmp(key, "use"))
    {
      cfg->use = (unsigned char) tolower(value[0]);
    }
    else if (!strcmp(key, "next"))
    {
      cfg->next = (unsigned char) tolower(value[0]);
    }
    else if (!strcmp(key, "prev"))
    {
      cfg->prev = (unsigned char) tolower(value[0]);
    }
    else if (!strcmp(key, "inv"))
    {
      cfg->inv = (unsigned char) tolower(value[0]);
    }
    else
    {
      write_log(LOG_INFO, "%s:\n\tInvalid key: %s\n", __func__, key);
      continue;
    }
  }
}

// Initialises the config struct by reading the configuration file
void
read_config(void)
{
  FILE * f_cfg;
  char * filepath;

  cfg = malloc(sizeof(CONFIG));
  filepath = get_config_path();

  init_config();
  f_cfg = fopen(filepath, "r");
  if (f_cfg == NULL)
  {
    write_log(LOG_INFO, "%s:\n\tUnable to open file %s, creating new one\n",
              __func__, filepath);
    write_log(LOG_VERBOSE, "\tWriting default config to %s\n", filepath);
    write_config();
  }
  else
  {
    write_log(LOG_VERBOSE, "%s:\n\tReading configuration from %s\n", __func__,
              filepath);
    parse_config(f_cfg);
    fclose(f_cfg);
    f_cfg = NULL;
  }

  write_log(LOG_DEBUG, "\tcfg->up: %d\n\tcfg->down: %d"
            "\n\tcfg->left: %d\n\tcfg->right: %d\n\tcfg->use: %d"
            "\n\tcfg->next: %d\n\tcfg->prev: %d\n\tcfg->inv: %d\n",
            cfg->up, cfg->down, cfg->left, cfg->right, cfg->use, cfg->next,
            cfg->prev, cfg->inv);
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
  fprintf(f_cfg, "up = \"%c\"\n", cfg->up);
  fprintf(f_cfg, "down = \"%c\"\n", cfg->down);
  fprintf(f_cfg, "left = \"%c\"\n", cfg->left);
  fprintf(f_cfg, "right = \"%c\"\n", cfg->right);
  fprintf(f_cfg, "use = \"%c\"\n", cfg->use);
  fprintf(f_cfg, "next = \"%c\"\n", cfg->next);
  fprintf(f_cfg, "prev = \"%c\"\n", cfg->prev);
  fprintf(f_cfg, "inv = \"%c\"\n", cfg->inv);
  fclose(f_cfg);
  f_cfg = NULL;
  _free(filepath);
}
