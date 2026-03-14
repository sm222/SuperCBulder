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

# define NO_CONFIG_FILE "no config file found, \
 you want to make one or continue whit out it?"

# define WITCH_FILE "multiple file found, witch one want to be use"
# define WITCH_FILE_QUESTION "[c] continue | [m] make a config file | \
 else stop"


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
  "PROG",
  "LIB",
  "BUPLIB",
  0x0,
};

# define MAX_VAR_NAME_LEN (PATH_MAX * 4)

typedef struct configValue {
  int      fd;
  char*    name;
  char**   rawData;
  char buffer[MAX_VAR_NAME_LEN];
} t_configValue;

enum varReserveName {
  Vcc,
  Vcxx,
  Vname,
  Vnamex,
  VCFLAGS,
  VCXXFLAGS,
  Ving,
  Vprog,
  Vlib,
  Vpublib,
};

typedef struct s_reserveVar {
  size_t size; //? set size too reserveVarName
  bool  varVAlue[sizeof(reserveVarName) / sizeof(char*)];
} t_reserveVar;

typedef struct {
  bool        cpp;
  int         fd;
  t_SCB*      scb;
  int         outputType;
  char*       workingDirectory;
  char*       config[PATH_MAX];
  t_outVar*   outVar;
  //
  t_reserveVar  var;
  t_configValue configFile;
} outFileData;

int  superStrcmp(const char* s1, const char* s2, size_t n);

outFileData  makerSetup(t_SCB* in, int mode);
int          makerStart(outFileData* data);

size_t      output(int fd, const char* s, ...);
size_t      header(int fd, const char* comment, const char* uName, const char* pName, const char* fType);

bool        newFile(char* name, outFileData* data);
void        closeFile(outFileData* data);

char*       findCommentFromType(int type);


t_outVar*   makeOutVarLast(const char* name, t_outVar** list);
void        freeOutVar(t_outVar** list);
void        printOutVar(t_outVar* head);

int         isVarInConfig(int var, t_reserveVar varList);

char*       readVariable(outFileData* data, int var);
char*       readVariableName(outFileData* data, const char* name);

int         removeEndl(char* value);


int         printConfigFiles(t_node* head);

#endif