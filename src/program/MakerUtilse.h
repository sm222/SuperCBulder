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

# define NO_CONFIG_FILE "no config file found," \
  " you want to make one or continue whit out it?"

# define WITCH_FILE "multiple file found, witch one want to be use"
# define WITCH_FILE_QUESTION "[c] continue | [m] make a config file |" \
  " else stop"

# define MULT_COMPILE_RULE "scb: multiple compile rule define," \
  " only one at the time can be use\n"

enum {
  makefile = 0,
  bash     = 1,
  cmake    = 2,
};

static const char* const buildFileLanguage[] = {
  "Makefile:#",
  "bash:#",
  "?",
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
  "SHELL",
  0x0,
};


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

int  superStrcmp(const char* s1, const char* s2, size_t n);

outFileData  makerSetup(t_SCB* in, int mode);
int          makerStart(outFileData* data);

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