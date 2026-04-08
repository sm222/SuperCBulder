# include "flags.h"
# include "utils.h"
# include <string.h>
# include <strings.h>
# include <stdlib.h>


static t_flagValue* fv_add(int flag, const char* value) {
  t_flagValue* f = calloc(1, sizeof(*f));
  if (f) {
    f->flag = flag;
    bzero(f->name, FLAG_NAME_LEN * sizeof(char));
    f->value = d__strdup(value);
    if (!f->value) {
      free(f);
      f = NULL;
    }
  }
  return f;
}

int fv_add_last(t_flagValue** list, int flag, const char* value) {
  if (!list)
    return -1;
  if (!(*list)) {
    *list = fv_add(flag, value);
    if (!(*list))
      return -2;
  }
  else {
    t_flagValue *tmp = (*list);
    for (; tmp && tmp->next; tmp = tmp->next) { }
    tmp->next = fv_add(flag, value);
    if (!tmp->next)
      return -2;
  }
  return 0;
}

int fv_free(t_flagValue** list) {
  if (!list)
    return -1;
  for (t_flagValue* tmp = (*list); tmp; ) {
    free(tmp->value);
    t_flagValue *t = tmp->next;
    free(tmp);
    tmp = t;
  }
  return 0;
}

int fv_set_name(t_flagValue* node, const char* name) {
  if (!node)
    return 1;
  const size_t l = strlen(name);
  memcpy(node->name, name, l > FLAG_NAME_LEN - 1 ? FLAG_NAME_LEN -1 : l);
  node->name[FLAG_NAME_LEN - 1] = 0;
  return 0;
}

# include <stdio.h>
void fv_print(t_flagValue* list) {
  for ( ; list; list = list->next) {
    printf("[%d]%s\n", list->flag, list->value);
  }
}