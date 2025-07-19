#ifndef  NODE_H 
# define NODE_H

# include "program.h"
# include <stdlib.h> // calloc free


t_FilesList* makeNode(char* name, int type);
int freeNode(t_FilesList** list);
t_FilesList* makeNodeLast(char* name, int type, t_FilesList** list);

#endif