#include "node.h"
#include <string.h>

size_t getId(int mode) {
  static size_t id = 0;
  return mode ? ++id: id;
}

static t_FilesList* makeNode(char* name, int type) {
  if (!name)
    return NULL;
  t_FilesList* node = NULL;
  node = calloc(1, sizeof(*node));
  if (!node)
    return NULL;
  node->data.name = strdup(name);
  node->data.type = type;
  node->data.id = getId(1);
  return node;
}


// -1 missing name
// -2 no list provide
// -3 faild to make a new node
// else return len of list
t_FilesList* makeNodeLast(char* name, int type, t_FilesList** list) {
  if (!list) // look for list first 
    return NULL;
  if (!name)
    return NULL;
  int size = 0;
  t_FilesList* head = *list;
  t_FilesList* made = NULL;
  while (head) {
    size++;
    if (!head->next)
      break ;
    head = head->next;
  }
  // make head
  if (!size) {
    *list = makeNode(name, type);
    if (!(*list))
      return NULL;
    made = *list;
  }
  else {
    // add tale
    head->next = makeNode(name, type);
    if (!head->next)
      return NULL;
    made = head->next;
    made->prev = head;
  }
  return made;
}


int freeNode(t_FilesList** list) {
  if (!list) {
    return -1;
  }
  int size = 0;
  t_FilesList* head = *list;
  while (head) {
    if (head->child) {
      size += freeNode(&head->child);
    }
    t_FilesList* next = head->next;
    free(head->data.name);
    free(head);
    head = next;
    size++;
  }
  *list = NULL;
  return size;
}

size_t  getNodeLen(t_FilesList* head) {
  size_t len = 0;
  //
  if (!head)
    return 0;
  for (; head && head->prev; head = head->prev) { ; }
  for (; head; head = head->next) { len++; }
  return len;
}