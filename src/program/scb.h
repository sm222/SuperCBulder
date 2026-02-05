#ifndef  SCB_H
# define SCB_H

# include "../dataType.h"
# include "../utilse.h"
# include "node.h"
# include <dirent.h>
# include <sys/stat.h>
# include "fileType.h"

enum {
  verbose,
};

typedef struct {
  char          path[PATH_MAX + 1];
  char          configPath[PATH_MAX + 1];
  unsigned int  maxDep;
  int32_t       flags;
  int           error;
} t_SCB;

#endif