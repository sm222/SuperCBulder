
#include "dataType.h"
# include "utils.h"
#include "parsing.h"

# ifdef NAME_CHECK
static bool test_name(const char* name) {
  const char sep = FILE_SEP;
  const char* cut = strrchr(name, sep);
  if (!cut || !cut[0]) {
    return false;
  }
  const size_t nameLen = strlen(cut);
  if (nameLen < 1) {
    return false;
  }
  if (strcmp(cut + 1, PROG_NAME) != 0) {
    put_str_nl("error: program was renamed!", STDERR_FILENO);
    return false;
  }
  return true;
}
# endif

int setStart(void*);

static int base(t_mainData data, int fdIn, int fdOut) {
  int status = EX_OK;
  t_settings programSettings = {
    .stdIn        = fdIn,        //*
    .stdOut       = fdOut,       //
    .ac           = data.ac,     //
    .current      = 1,           // skip program name at index 0
    .jump         = 1,           //
    .av           = data.av,     //
    .programName = data.av[0],   // program name
    .flags        = 0,           // flags
    .env          = data.env,    //
    .flagValue    = NULL,        //
    .avFt         = NULL,        //
    .programFt    = NULL,        //
    .ftsingle     = NULL,        //
    .ftdouble     = NULL,        //*
  };
  //
  # define SETUP_EXTERN
  # ifdef NAME_CHECK
  if (!test_name(data.av[0]))
    return 1;
  # endif
  # ifdef SETUP_EXTERN
  if (setStart(&programSettings))
    return 1;
  # endif
  env_parsing(&programSettings);
  av_setup(&programSettings.avNoFlags, programSettings.ac);
  bool error = false;
  for (; programSettings.current < programSettings.ac; \
  programSettings.current += programSettings.jump) {
    if (strncmp(programSettings.av[programSettings.current], "--", 2) == 0) {
      if (programSettings.ftdouble) {
        status = programSettings.ftdouble(&programSettings);
      } else {
        status = parsing_get_double(&programSettings);
      }
    }
    else if (programSettings.av[programSettings.current][0] == '-') {
      if (programSettings.ftsingle) {
        status = programSettings.ftsingle(&programSettings);
      } else {
        status = parsing_get_single(&programSettings);
      }
    } else {
      if (programSettings.avFt) {
        status = programSettings.avFt(&programSettings, programSettings.av[programSettings.current]);
      } else {
        av_add(&programSettings.avNoFlags, programSettings.av[programSettings.current]);
      }
    }
    error = read_byte(programSettings.flags, setting_continue_on_error);
    if (error && status)
      programSettings.current = programSettings.ac;
    put_str_error(&programSettings, RED, "code %d", status);// debug only
  }
  // programe here
  if (programSettings.programFt && !(error && status))
    status = programSettings.programFt(&programSettings);
  av_free(&programSettings.avNoFlags);
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
