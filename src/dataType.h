
#ifndef  DATA_TYPE_H
# define DATA_TYPE_H

# include <error.h>
# include <sysexits.h>
# include <string.h>
# include <strings.h>
# include <errno.h>
# include <stdio.h>
# include <unistd.h>
//# include <ncurses.h>
# include <stdbool.h>
# include <sys/param.h>
# include "argv.h"


// https://stackoverflow.com/questions/142508/how-do-i-check-os-with-a-preprocessor-directive

# define SYS_LINUX   0 // Linux is use
# define SYS_WIN     1 // Windows is use
# define SYS_MAC     2 // Mac is use
# define SYS_UNKNOW  3 // add more later

# if (unix || __unix || __unix__ || __linux__)
#  define SYSTYPE SYS_LINUX
#  define FILE_SEP '/'
# elif (_WIN32 || _WIN64 || __CYGWIN__)
#  define SYSTYPE SYS_WIN
#  define FILE_SEP '\\'
# elif (__APPLE__ || __MACH__)
#  define FILE_SEP '/'
#  define SYSTYPE SYS_MAC
# else
#  define SYSTYPE -1
#  error  "Unsupported system type"
# endif

# ifndef FILE_SEP
#  error  "no file separator define"
# endif

# ifndef COLORS
#  define COLORS
#  define CS  "\001"   //*start code
#  define CE  "\002"   //*end   code
#  define RED    CS "\e[31m" CE
#  define GRN    CS "\e[32m" CE
#  define YEL    CS "\e[33m" CE
#  define BLU    CS "\e[34m" CE
#  define MAG    CS "\e[35m" CE
#  define CYN    CS "\e[36m" CE
#  define WHT    CS "\e[37m" CE
#  define ORG    CS "\e[38;5;202m" CE
#  define PIK    CS "\e[38;5;176m" CE
#  define RESET  CS "\e[0m\022" CE
#  define CLE    CS "\e[1;1H\x1b[2J" CE
#  define MAX_COLORLEN 20
# endif


/*
* Placeholders bellow
* replace by additional settings if need be
*/
enum {
  setting_color = 1,
  setting_continue_on_error = 2,
  setting_1 = 4,
  setting_2 = 8,
  setting_3 = 16,
  setting_4 = 32,
  setting_5 = 64,
};

typedef struct mainData {
  const int          ac;
  const char* const* av;
  const char* const* env;
  //
} t_mainData;

# include "flags.h"

typedef  int (*avProcess)(void* , const char*);
typedef  int (*avSingle)(void*);
typedef  int (*avDouble)(void*);

typedef  int (*program)(void*);

typedef struct settings {
  const int          stdIn;
  const int          stdOut;
  const int          ac;
  int                current;
  int                jump;
  const char* const* av;
  const char*        programName;
  t_avData           avNoFlags;
  int32_t            flags;
  const char* const* env;
  t_flagValue*       flagValue;
  avProcess          avFt;
  program            programFt;
  avSingle           ftsingle;
  avDouble           ftdouble;
} t_settings;




#endif