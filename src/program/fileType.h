#ifndef  FILE_TYPE_H
# define FILE_TYPE_H

typedef enum {
  unknown = -1,
  folder,
  cFile,
  hFile,
  cppFile,
  hppFile,
  tppFile,
} t_file_type;


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

#endif