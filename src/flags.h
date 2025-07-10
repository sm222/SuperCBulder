#ifndef __FLAGS__
# define __FLAGS__

//  flags like -xyz

//  single
/*
format
-a             -    what it dose
^^ flag
| one dash
*/

# define DEVFLAG 1 // use only to work on base

enum valueArg {
  e_bool,
  e_next,
  e_equal,
};


#  if DEVFLAG
static const char* const singleFlags[] = {
  "-c             -  turn on color",
  "-s             -  contine parsing if invalid flags",
  "-h             -  show this message",
  "-p             -  run the program in this folder as root",
  0x0, // NULL
};

enum {
  e_color = 0,
  e_stopError,
  e_info,
  e_help,
  e_path,
};


//  verbose
static const char* const verboseFlags[] = {
  "--color        -   same as c",
  "--stopOnError  -   same as s",
  "--info         -   todo     ",
  "--help         -   same as h",
  "--path={path}  -   run the program in this folder as root",
  0x0, // NULL
};

# endif

#  if DEVFLAG == 0
static const char* const singleFlags[] = {
  "-c             -  turn on color",
  "-s             -  contine parsing if invalid flags",
  "-h             -  show this message",
  0x0, // NULL
};

enum {
  e_color = 0,
  e_stopError,
  e_info,
  e_help,
  e_dest,
};


//  verbose
static const char* const verboseFlags[] = {
  "--color        -   same as c",
  "--stopOnError  -   same as s",
  "--help         -   same as h",
  0x0, // NULL
};
# endif

#endif