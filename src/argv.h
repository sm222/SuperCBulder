#ifndef  ARGV_H
# define ARGV_H

#include <stdio.h>

typedef struct {
  char** data;
  size_t amount;
} t_avData;

int av_setup(t_avData* input, size_t size);

int av_add(t_avData* input, const char* av);

char* av_read(t_avData* input, size_t i);

size_t av_len(t_avData* input);

int av_free(t_avData* input);

#endif