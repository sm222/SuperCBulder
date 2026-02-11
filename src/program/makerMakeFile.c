# include "MakerUtilse.h"

# include <ctype.h>

static void drawVarName(const char* name, const int* fd) {
  size_t i = 0;
  const size_t nameLen = strlen(name);
  char outBuffer[(nameLen * 2) + 10];
  size_t rptr = 2;
  memcpy(outBuffer, "F_", 2);
  while (name[i]) {
    outBuffer[rptr] = (char)toupper(name[i]);
    i++;
    rptr++;
  }
  memcpy(outBuffer + rptr, "\t=\t\n", 5);
  write(*fd, outBuffer, strlen(outBuffer));
}

static void  buidFileAndFolder(t_node* head, const int* fd) {
  t_node* tmp = head;
  while (tmp) {
    if (tmp->data.type == folder) {
      drawVarName(tmp->data.name, fd);
      buidFileAndFolder(tmp->child, fd);
    }
    tmp = tmp->next;
  }
}

static ssize_t readList(t_node* head, outFileData* data) {
  const int fd = data->fd;
  buidFileAndFolder(head, &fd);
  return 0;
}

ssize_t buildMakefile(outFileData* data) {
  ssize_t totalBytes = 0;
  const char* hardcodeName = "sm222";//! change later
  const char* hardcodePname = "scb" ;//!
  if (!newFile("Makefile", data))
    return -1;
  totalBytes += header(data->fd, "#", hardcodeName, hardcodePname, "Makefile");
  readList(data->header, data);
  closeFile(data);
  return totalBytes;
}