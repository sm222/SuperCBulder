#ifndef  UTILS_H
# define UTILS_H

# include "dataType.h"

# ifndef  STR_BUFF_LEN
#  define STR_BUFF_LEN 300
# endif

size_t getArrayLen(const char* const* array);

char* d__strdup(const char* s);
//
void put_str(const char* str, int fd, bool nl);
void put_str_nl(const char* str, int fd);
//
void set_byte(int32_t* flag, int32_t pos, bool status);
bool read_byte(int32_t flag, int32_t value);
void print_bits(int32_t var);

/**
*
*
*
**/
void put_str_error(t_settings* setting, const char* color, const char* str, ...);

#endif