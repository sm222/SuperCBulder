# include "program.h"
# include <fcntl.h>

static size_t output(const char* line, int fd, bool jump) {
  const size_t total = write(fd, line, strlen(line));
  if (!jump)
    return total;
  return total + write(fd, "\n", 1);
}


#include <time.h>

static size_t header(t_setting* setting, int fd) {
  (void)setting;
  time_t rawtime;
  struct tm* timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  size_t out = 0;
  out =  output("############################", fd, true);
  out += output("# MakeFile made with scb   #", fd, true);
  out += output("#                          #", fd, true);
  out += output("# ", fd, false);
  out += output(asctime (timeinfo), fd, false);
  out += output("############################", fd, true);
  return out;
}

static int openFile(const char* name) {
  const int fd = open(name, O_CREAT | O_TRUNC | O_RDWR, 0744);
  if (fd == -1) {
    fprintf(stderr, "scb: can't make/open %s\n", name);
    return 0;
  }
  return fd;
}

static int closeFile(int fd) {
  return close(fd);
}

static void printAll(t_setting* setting, t_FilesList* root, int fd) {
  (void)setting;
  for (t_FilesList* l = root; l != NULL; l = l->next) {
    output(l->data.name, fd, true);
  }
}

int build(t_setting* setting, t_FilesList* root) {
  int error = 0;
  int file = 0;
  size_t out = 0;
  if (!root) {
    fprintf(stderr, "scb: no fille provided\n");
    return ++error;
  }
  file = openFile("Makefile");
  if (!file)
    return ++error;
  out += header(setting, file);
  printAll(setting, root, file);
  return closeFile(file);
}
