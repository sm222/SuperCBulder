#ifndef  PROGRAM_H
# define PROGRAM_H

# include "dataType.h"
# include "utilse.h"

// add lib type for unix and windows

enum {
  unknown = -1,
  folder,
  cFile,
  hFile,
  cppFile,
  hppFile,
  tppFile,
};

static const char* const fileType[] = {
  "unknown",
  "folder",
  ".c",
  ".h",
  ".cpp",
  ".hpp",
  ".tpp",
  0x0,
};

static const char* const ignoreFolder[] = {
  "/.git",
  "/.vscode",
  0x0,
};

//  node
t_FilesList* makeNode(char* name, int type);
t_FilesList* makeNodeLast(char* name, int type, t_FilesList** list);
int          freeNode(t_FilesList** list);
/*

*/
int          program(t_setting* setting);

#endif