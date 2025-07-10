
#include "dataType.h"
#include "utilse.h"
#include "parsing.h"
#include "program.h"

bool test_name(const char* name) {
  # if (SYSTYPE == SYS_LINUX) || (SYSTYPE == SYS_MAC)
    const char sep = '/';
  # endif
  # if SYSTYPE == SYS_WIN
    const char sep = '\\';
  # endif
  # if SYSTYPE == -1 // safety
    const char sep = '/';
  # endif
  const char* cut = strrchr(name, sep);
  if (!cut || !cut[0]) {
    return false;
  }
  const size_t nameLen = strlen(cut);
  if (nameLen < 1) {
    return false;
  }
  if (strcmp(cut + 1, PROG_NAME) != 0) {
    write(2, "error: program was rename!\n", 27);
    return false;
  }
  return true;
}


int base(t_mainData data, int fdIn, int fdOut) {
  int status = EX_OK;
  t_setting programSetting;
  //
  memset(&programSetting, 0 ,sizeof(programSetting));
  programSetting.stdIn = fdIn;
  programSetting.stdOut = fdOut;
  programSetting.programeName = data.av[0];
  programSetting.stopOnError = 1; //defalut
  programSetting.ac = data.ac;
  programSetting.av = data.av;
  //
  # ifdef NAME_CHECK
  if (!test_name(data.av[0]))
    return 1;
  # endif
  //const size_t envLen  = getArrayLen(data.env);
  // skip program name
  int* i = &programSetting.currentArg;
  for (*i = 1; *i  < data.ac; (*i)++) {
    const size_t lineSize = strlen(data.av[*i]);
    if (strncmp(data.av[*i], "--", 2) == 0 )                             // verbose
      status = setVerboseFlag(data.av[*i] + 2, &programSetting);
    else if (strncmp(data.av[*i], "-", 1) == 0 && data.av[*i][1] != '\0') // flags like -xyz
      status = setFlag(data.av[*i], lineSize, &programSetting);
    else {
      printf("arg ->%s\n", data.av[*i]); // normal args // replace for your code
    }
    if (programSetting.help) {
      help(programSetting.helpFlag);
    }
    if (programSetting.stopOnError && status)
      break ;
  }
  if (!status)
    status = program(&programSetting);
  return status;
}


int main(int ac, const char* const* av, const char* const* env) {
  t_mainData data = {
    .ac = ac,
    .av = av,
    .env = env,
  };
  const int error = base(data, STDIN_FILENO, STDOUT_FILENO);
  return error;
}
