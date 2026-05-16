#ifndef  PARSING_H
# define PARSING_H

# include "dataType.h"

# ifndef PROG_NAME
#  define PROG_NAME "DefaultProgramName"
# endif

enum {
  none = -1,
  equal = 0,
  next  = 1
};

char* grab_value(t_settings* settings, const char* value, int type);

int env_parsing(t_settings* setting);
//
int parsing_get_single(t_settings* setting);
int parsing_get_double(t_settings* setting);

#endif