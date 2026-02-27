#include "parsing.h"
# include "dataType.h"
# include "utilse.h"
# include "flags.h"
# include "prossess.h"

const char* const flag_singile[] = {
  "c\tforce colors output",
  0x0,
};

# define AUTO_COLOR "AUTO_COLOR=TRUE"

int env_parsing(t_setting* setting) {
  for (int i = 0; setting->env[i]; i++) {
    if (strncmp(setting->env[i], AUTO_COLOR, strlen(AUTO_COLOR)) == 0) {
      set_byte(&setting->flags, setting_color, true);
      break ;
      // only one setting so break after we find it to optimise run time
    }
  }
  return 0;
}

enum {
  none = -1,
  equal = 0,
  next  = 1
};

# define VAR_NAME_MAX_SIZE  NAME_MAX
# define VALUE_MAX_SIZE     PATH_MAX


char   value__[VALUE_MAX_SIZE];

# include <string.h>


static char* grab_value(t_setting* setting, const char* value, int type) {
  bzero(&value__, VALUE_MAX_SIZE);
  const char*  p = value;
  if (!p)
    return value__;
  if (type == next) {
    if (setting->current + setting->jump >= setting->ac) {
      return value__;
    }
    p = setting->av[setting->current + setting->jump];
    setting->jump += 1; // skip one arg in av
  }
  const size_t len = strlen(p);
  const size_t copylen = len < VALUE_MAX_SIZE ? len : VALUE_MAX_SIZE - 1;
  memcpy(value__, p, copylen);
  return value__;
}

static int set_single_value(t_setting* setting, int c) {
  if (c == 'c') {
    set_byte(&setting->flags, setting_color, true);
  }
  else if (c == 'h') {
    help(setting, "");
  }
  else if (c == 'B') {
    const char* v = grab_value(setting, "", next);
    barr(setting, v);
  }
  else {
    put_str_error(setting, RED, "%c: is unknow flag, call -h or --help to see the option\n", c);
    return 1;
  }
  return 0;
}

int parsing_get_single(t_setting* setting) {
  setting->jump = 1;
  if (setting->current > setting->ac)
    return -1;
  if (setting->av[setting->current][0] == '-' && !setting->av[setting->current][1]) {
    put_str_error(setting, RED, "flag was call with not params\n");
    return 2;
  }
  const size_t len = strlen(setting->av[setting->current]);
  int error = 0;
  for (size_t i = 1; i < len; i++) {
    error = set_single_value(setting, setting->av[setting->current][i]);
    if (!read_byte(setting->flags, setting_continue_on_error) && error)
      break;
  }
  return error;
}

/*
  double
*/

static bool strncmp_name(const char* s1, const char* s2) {
  const size_t l = strlen(s2);
  if (strncmp(s1, s2, l) == 0 && (s1[l] == '\0' || s1[l] == '='))
    return 1;
  return 0;
}





# include <stdlib.h>


static int parsing_value_double(t_setting* setting, const char* name, const char* value) {
  int(*ft)(t_setting*, const char*) = NULL;
  int grabValue = none;
  //
  if (strncmp_name(name, "color"))
    set_byte(&setting->flags, setting_color, true);
  else if (strncmp_name(name, "foo")) {
    ft = &foo;
    grabValue = equal;
  }
  else if (strncmp_name(name, "help"))
    ft = &help;
  else if (strncmp_name(name, "barr")) {
    ft = &barr;
    grabValue = next;
  }
  else {
    put_str_error(setting, RED, "%s: --%s unknow flag, try --help", setting->av[0], name);
    return 1;
  }
  grab_value(setting, value, grabValue);
  const int e = ft ? ft(setting, value__): 0;
  return e;
}


static int set_double_value(t_setting* setting) {
  const char* s = setting->av[setting->current] + 2;
  size_t len = 0;
  while (s[len] && (s[len] != 0 && s[len] != '=')) {
    len++;
  }
  return parsing_value_double(setting, s, s + len);
}

int parsing_get_double(t_setting* setting) {
  setting->jump = 1;
  if (setting->current > setting->ac)
    return -1;
  if (setting->av[setting->current][1] == '-' && !setting->av[setting->current][2]) {
    put_str_error(setting, RED, "flag was call with not params\n");
    return 2;
  }
  return set_double_value(setting);
}