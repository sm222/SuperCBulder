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

typedef struct configValue {
  /*
  EE
  */
} t_configValue;

typedef struct {
  bool        cpp;
  int         fd;
  int         configFd;
  char        configFilename[PATH_MAX];
  char*       projectname;
  size_t      varByte;
  size_t      varArray;
  t_SCB*      scb;
  int         outputType;
  char*       workingDirectory;
  char        cCompiler[100];
  char        cppCompiler[100];
  char*       config[PATH_MAX];
  t_outVar*   outVar;
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


int         printConfigFiles(t_node* head);

#endif