#ifndef  NODE_H
# define NODE_H


# include "../dataType.h"
# include <stdlib.h> // calloc free


typedef struct  {
  char*  name;
  int    type;
  size_t fsize;
  size_t id;
} t_node_data;

typedef struct s_node {
  struct s_node* next;
  struct s_node* child;
  t_node_data    data;
} t_node;

# define  IS_FOLDER(node)        (node->data.type == folder)
# define  IS_FOLDER_EMPTY(node)  ((IS_FOLDER(node) && node->data.fsize == 0))
# define  IS_FILE(node)          (!IS_FOLDER(node))
# define  IS_C_CPP(node)         (!IS_FOLDER(node) && (node->data.type % 2 == 0))
# define  IS_C(node)             (node->data.type == cFile)
# define  IS_CPP(node)           (node->data.type == cppFile)
# define  IS_SCB(node)           (node->data.type == configFile)


int          freeNode(t_node** list);
t_node*      makeNodeLast(char* name, int type, t_node** list);
size_t       getNodeLen(t_node* head);
void*        dellNode(t_node* n);

#endif