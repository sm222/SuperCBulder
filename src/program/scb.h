#ifndef  SCB_H
# define SCB_H

# include "../dataType.h"
# include "../utilse.h"
# include "node.h"
# include <dirent.h>
# include <sys/stat.h>
# include "fileType.h"


typedef struct {
  t_setting*    mainData;
  char          path[PATH_MAX + 1];
  char          originPath[PATH_MAX + 1];
  char          configPath[PATH_MAX + 1];
  unsigned int  maxDep;
  int32_t       flags;
  int           error;
  t_node*       node;
} t_SCB;

#endif