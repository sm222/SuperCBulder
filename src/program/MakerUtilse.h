#ifndef  MAKE_UTISE_H
# define MAKE_UTISE_H

# include "node.h"
# include "../dataType.h"
# include "../utilse.h"
# include "fileType.h"
# include <fcntl.h>
# include "scb.h"
# include <stdarg.h>

# define ENDL(c)    ((c == '\n') || (c == '\0'))

# define TOKENSIZE 1
# define B

# define NO_CONFIG_FILE "no config file found," \
  " you want to make one or continue whit out it?"

# define WITCH_FILE "multiple file found, witch one want to be use"
# define WITCH_FILE_QUESTION "[c] continue | [m] make a config file |" \
  " else stop"

# define MULT_COMPILE_RULE "scb: multiple compile rule define," \
  " only one at the time can be use\n"

# define UNKNOWN_TYPE "scb: file type unknown\ntype was [%d]\n"
# define ERROR_FILE "error while parsing\ncode was [%d]\n"

enum {
  L_unknown  = -1,
  L_empty    =  0,
  L_comment  =  1,
  L_var      =  2,
  L_varValue =  3,
  L_invalid  =  4,
};



enum {
  makefile =  0,
  sh       =  1,
  cmake    =  2,
  notype   = -1,
};

static const char* const buildFileLanguage[] = {
  "Makefile:#",
  "sh:#",
  "?:#",
  0x0,
};

typedef struct s_outVar {
  struct s_outVar* next;
  char*            name;
} t_outVar;

static const char* const reserveVarName[] = {
  "CC",
  "CXX",
  "NAME",
  "NAMEX",
  "CFLAGS",
  "CXXFLAGS",
  "ING",
  "DEP",
  "PROG",
  "LIB",
  "DLIB",
  "SHELL",
  0x0,
};

//ar rcs

/*
! no tokens can ever be put in those string
! getValue from readVariableName will not follow token if
! the default value is read
*/
static const char* const reserveVarNameDefaultValue[] = {
  "cc",                   // cc
  "c++",                  // cxx
  "",                     //name
  "",                     //namex
  "-Wall -Werror -Wextra",//ccflags
  "-Wall -Werror -Wextra",//cxxflags
  "",                     //ing
  "",                     //dep
  "",                     //prog
  "",                     //lib
  "",                     //dlib
  "sh",                   //shell
  0x0,
};

typedef enum {
  Vcc,
  Vcxx,
  Vname,
  Vnamex,
  VCFLAGS,
  VCXXFLAGS,
  Ving,
  Vdep,
  Vprog,
  Vlib,
  Vdlib,
  Vshell,
} e_reserveVarName;

static const char* const keyWords[] = {
  "LINUX ",
  "WINDOWS ",
  "MACOS ",
  "ENV_",
  "SHELL ",
  0x0,
};

//*Static Library
// ar rcs name|namex files
//
//Shared (Dynamic) 
//gcc -c -fPIC mylib.c -o mylib.o
//gcc -shared -o libmylib.so mylib.o
//
//* https://www.geeksforgeeks.org/c/how-do-i-create-a-library-in-c/

enum {
  k_linux   = 0,
  k_windows = 1,
  k_macos   = 2,
  k_env     = 3,
  k_shell   = 4,
};

//3 os, strat at 0 in keyWords
# define NUMBER_OF_OS 3

# define MAX_VAR_NAME_LEN (PATH_MAX * 4)

typedef struct configValue {
  int      fd;
  char*    name;
  char**   rawData;
  char buffer[MAX_VAR_NAME_LEN];
} t_configValue;


typedef struct s_reserveVar {
  size_t size; //? set size too reserveVarName
  bool  varVAlue[sizeof(reserveVarName) / sizeof(char*)];
} t_reserveVar;

typedef void(*shellCall)(void*, ssize_t*, const char*);

typedef struct {
  bool        cpp;
  int         fd;
  t_SCB*      scb;
  short       target; // linux, windows, mac
  int         outputType;
  char*       workingDirectory;
  char*       config[PATH_MAX];
  t_outVar*   outVar;
  //
  t_reserveVar  var;
  t_configValue configFile;
  shellCall     shellFt;
} outFileData;

outFileData  makerSetup(t_SCB* in, int mode);
int          makerStart(outFileData* data);

short       printNl(const int fd);
size_t      output(int fd, const char* s, ...);
size_t      header(outFileData* data, const char* comment, const char* uName, const char* pName, const char* fType);

bool        newFile(char* name, outFileData* data);
void        closeFile(outFileData* data);

char*       findCommentFromType(int type);


void        extractVar(const char* l, size_t start, size_t *end, char const sep);

t_outVar*   makeOutVarLast(const char* name, t_outVar** list);
void        freeOutVar(t_outVar** list);
void        printOutVar(t_outVar* head);

int         isVarInConfig(int var, t_reserveVar varList);

char*       readVariableName(outFileData* data, e_reserveVarName name);

int         removeEndl(char* value);


int         printConfigFiles(t_node* head);

#endif