#include "node.h"
#include <string.h>



static t_node* makeNode(char* name, int type) {
  if (!name)
    return NULL;
  t_node* node = NULL;
  node = calloc(1, sizeof(*node));
  if (!node)
    return NULL;
  node->data.name = strdup(name);
  node->data.type = type;
  return node;
}


// -1 missing name
// -2 no list provide
// -3 faild to make a new node
// else return len of list
t_node* makeNodeLast(char* name, int type, t_node** list) {
  if (!list) // look for list first 
    return NULL;
  if (!name)
    return NULL;
  int size = 0;
  t_node* head = *list;
  t_node* made = NULL;
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
  }
  return made;
}


int freeNode(t_node** list) {
  if (!list) {
    return -1;
  }
  int size = 0;
  t_node* head = *list;
  while (head) {
    if (head->child) {
      size += freeNode(&head->child);
    }
    t_node* next = head->next;
    free(head->data.name);
    free(head);
    head = next;
    size++;
  }
  *list = NULL;
  return size;
}

size_t  getNodeLen(t_node* head) {
  size_t len = 0;
  //
  if (!head)
    return 0;
  for (; head; head = head->next) { len++; }
  return len;
}