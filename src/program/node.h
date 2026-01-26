#ifndef  NODE_H 
# define NODE_H


# include <stdlib.h> // calloc free

typedef struct FilesListData {
  char*  name;
  char*  parant;
  int    type;
  size_t size;
  size_t id;
} FilesListData;

typedef struct FilesList {
  FilesListData     data;
  //
  struct FilesList* child;
  struct FilesList* parant;
  //
  struct FilesList* next;
  struct FilesList* prev;
} t_FilesList;

size_t       getId(int mode);
int          freeNode(t_FilesList** list);
t_FilesList* makeNodeLast(char* name, int type, t_FilesList** list);
size_t       getNodeLen(t_FilesList* head);

#endif