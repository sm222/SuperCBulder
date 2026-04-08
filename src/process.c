#include "dataType.h"
# include "utils.h"

enum {
  none = -1,
  equal = 0,
  next  = 1
};

static bool valid_end(const char* s, int t) {
  if (t == none && (s[0]))
    return true;
  if (t == equal && (s[1] == '\0'))
    return true;
  if (t == next && (s[0] == '\0'))
    return true;
  return false;
}

static int foo_err(t_settings* setting, const char* data) {
  put_str_error(setting, RED, "%s: foo%s missing value", setting->av[0], data ? data : "");
  put_str_error(setting, RED, "ex: --foo=value");
  return 1;
}

int foo(t_settings* setting, const char* data) {
  if (valid_end(data, equal))
    return foo_err(setting, data);
  printf("hi %s\n", data);
  return 0;
}

static int help_err(t_settings* setting, const char* data) {
  (void)data;
  put_str_error(setting, RED, "%s: help take no value", setting->av[0]);
  put_str_error(setting, RED, "ex: --help");
  return 1;
}

int  help(t_settings* setting, const char* data) {
  if (valid_end(data, none))
    return  help_err(setting, data);
  printf("todo: help\n");
  return 0;
}


static int barr_err(t_settings* setting, const char* data) {
  (void)data;
  put_str_error(setting, RED, "%s: add take value", setting->av[0]);
  put_str_error(setting, RED, "ex: --help");
  return 1;
}

int  barr(t_settings* setting, const char* data) {
  if (valid_end(data, next))
    return  barr_err(setting, data);
  printf("add -> %s\n", data);
  return 0;
}