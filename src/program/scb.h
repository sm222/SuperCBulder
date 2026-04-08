#ifndef  SCB_H
# define SCB_H

# include "../dataType.h"
# include "../utils.h"
# include "node.h"
# include <dirent.h>
# include <sys/stat.h>
# include "fileType.h"


//scb [path] [type(make/bash/cmake)]


typedef struct {
  t_settings*    mainData;
  char          path[PATH_MAX + 1];
  char          originPath[PATH_MAX + 1];
  unsigned int  maxDep;
  int32_t       flags;
  int           error;
  t_node*       node;
  char*         buildType;
} t_SCB;

#endif