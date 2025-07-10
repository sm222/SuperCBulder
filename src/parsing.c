#include "dataType.h"
#include "utilse.h"
#include "flags.h"



#define ERR_MSG "%s: unknown flag "

static void flagsError(const char flag, t_setting* programSetting) {
  if (programSetting->color)
    fprintf(stderr, ERR_MSG "%s%c%s\n", programSetting->programeName, RED, flag, RESET);
  else
    fprintf(stderr, ERR_MSG "%c\n", programSetting->programeName, flag);
}

static void VerboseflagsError(const char* flag, t_setting* programSetting) {
  if (programSetting->color)
    fprintf(stderr, ERR_MSG "%s%s%s\n", programSetting->programeName, RED, flag, RESET);
  else
    fprintf(stderr, ERR_MSG "%s\n", programSetting->programeName, flag);
}


static ssize_t getVerboseFlagIndex(const char* line, t_setting* programSetting) {
  const ssize_t nbFlags = getArrayLen(verboseFlags);
  programSetting->flagval = NULL;
  for (ssize_t i = 0; i < nbFlags; i++) {
    const char* flag = verboseFlags[i] + 2; // skip the --
    programSetting->flagLen = 0;
    while (flag[programSetting->flagLen] != ' ' && \
    flag[programSetting->flagLen] != '=' && flag[programSetting->flagLen] != '>') {
      programSetting->flagLen++;
    }
    char copy[programSetting->flagLen + 2];
    memcpy(copy, flag, programSetting->flagLen);
    copy[programSetting->flagLen] = 0;
    if (flag[programSetting->flagLen] == '=' && line[programSetting->flagLen] == '=') {
      programSetting->rule[0] = e_equal;
      programSetting->flagval = line + programSetting->flagLen + 1; // +1 for '='
    }
    else if (flag[programSetting->flagLen] == '>' && programSetting->currentArg + 1 < programSetting->ac) {
      programSetting->rule[0] = e_next;
      programSetting->flagval = programSetting->av[programSetting->currentArg + 1];
      programSetting->currentArg += 1;
    }
    else {
      programSetting->flagval = NULL;
      if (flag[programSetting->flagLen] == '=')
        programSetting->rule[0] = -1;
      else if (flag[programSetting->flagLen] == '>')
        programSetting->rule[0] = -2;
    }
    if (strncmp(copy, line, programSetting->flagLen) == 0) {
      if (programSetting->rule[0] >= 0)
        return i;
      else {
        programSetting->helpFlag[0] = 2;
        programSetting->helpFlag[1] = i;
        programSetting->help = 1;
        fprintf(stderr, "miss use of '--%.*s flag'\n", programSetting->flagLen, flag);
        return -2;
      }
    }
  }
  return -1;
}

/*
*  not pretty but simple to add more
*/

int setVerboseFlag(const char* line, t_setting* programSetting) {
  memset(programSetting->rule, 0, sizeof(int) * 2);
  programSetting->flagval = NULL;
  const ssize_t i = getVerboseFlagIndex(line, programSetting);
  if (i == -1) {
    VerboseflagsError(line, programSetting);
    return 1;
  }
  else if (i == -2)
    return 1;
  if (i == e_color) {
    programSetting->color = 1;
  }
  if (i == e_stopError) {
    programSetting->stopOnError = 0;
  }
  if (i == e_info) {
    //todo
  }
  if (i == e_help) {
    programSetting->help = 1;
  }
  if (i == e_path) {
    snprintf(programSetting->workignDir, PATH_MAX, \
      "%s", programSetting->flagval);
  }
  return 0;
}

int setFlag(const char* line, const size_t n, t_setting* programSetting) {
  memset(programSetting->rule, 0, sizeof(int) * 2);
  programSetting->flagval = NULL;
  size_t i = 1;
  int status = 0;
  while (i < n) {
    if (line[i] == 'c') {
      programSetting->color = 1;
    }
    else if (line[i] == 's') {
      programSetting->stopOnError = 0;
    }
    else if (line[i] == 'h') {
      programSetting->help = 1;
      break ;
    }
    else {
      status = 1;
      flagsError(line[i], programSetting);
      if (programSetting->stopOnError)
        break ;
    }
    i++;
  }
  return status;
}