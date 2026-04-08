# include "dataType.h"
# include <stdlib.h>

char* d__strdup(const char* s) {
  if (!s)
    return NULL;
  size_t i = strlen(s);
  char *str = calloc(i + 1, sizeof(*str));
  if (!str)
    return NULL;
  memmove(str, s, i + 1);
  return str;
}

size_t getArrayLen(const char* const* array) {
  size_t i = 0;
  if (!array)
    return 0;
  while (array[i]) { i++; }
  return i;
}


void put_str(const char* str, int fd, bool nl) {
  write(fd, str, strlen(str));
  if (nl)
    write(fd, "\n", 1);
}


void put_str_nl(const char* str, int fd) {
  put_str(str, fd, true);
}


void set_byte(int32_t* flag, int32_t val, bool status) {
  if (!flag)
    return ;
  status ? (*flag |= val) : (*flag ^= val);
}

bool read_byte(int32_t flag, int32_t value) {
  return flag & value;
}

void print_bits(int32_t var) {
  int index = 32;
  char s[index + 1];
  s[index] = 0;
  for (int i = 1; index > 0; i <<= 1) {
    index--;
    s[index] = read_byte(var, i) + '0';
  }
  printf("%s\n", s);
}

#include <stdarg.h>
# include "utils.h"

void put_str_error(t_settings* setting, const char* color, const char* str, ...) {
  if (read_byte(setting->flags, setting_color)) {
    char b[50];
    sprintf(b, "%s*%s", color, RESET);
    put_str(b, STDERR_FILENO, false);
  }
  va_list list;
  va_start(list, str);
  char s[STR_BUFF_LEN + 1];
  vsnprintf(s, STR_BUFF_LEN, str, list);
  va_end(list);
  put_str(s, STDERR_FILENO, true);
}