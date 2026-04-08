#include "argv.h"
#include <stdlib.h>



int av_setup(t_avData* input, size_t size) {
  input->data = calloc(size + 1, sizeof(char*));
  if (!input->data)
    return 1;
  return 0;
}

int av_add(t_avData* input, const char* av) {
  if (!input)
    return 1;
  input->data[input->amount++] = (char*)av;
  return 0;
}

char* av_read(t_avData* input, size_t i) {
  if (!input || i > input->amount) {
    return NULL;
  }
  return input->data[i];
}

size_t av_len(t_avData* input) {
  return input ? input->amount : 0;
}

int av_free(t_avData* input) {
  if (input) {
    free(input->data);
    input->data = NULL;
    return 0;
  }
  return 1;
}