# include "makerBash.h"

static ssize_t drawVar(outFileData* data, int name) {
  const char* value = readVariableName(data, name);
  return output(data->fd, "%s=\"%s\"\n", reserveVarName[name], value);
}

static ssize_t drawCompiler(outFileData* data) {
  ssize_t total = 0;
  const char* const compile = data->cpp ? "$CXX" : "$CC";
  const char* const flags   = data->cpp ? "$CXXFLAGS" : "$CFLAGS";
  total += output(data->fd, "%s %s $file -o $NAME$NAMEX\n", compile, flags);
  return total;
}

static ssize_t drawNameAndStartVar(outFileData* data) {
  ssize_t total = 0;
  total += drawVar(data, Vcc);
  total += drawVar(data, Vcxx);
  total += printNl(data->fd);
  total += drawVar(data, Vname);
  total += drawVar(data, Vnamex);
  total += printNl(data->fd);
  total += drawVar(data, VCFLAGS);
  total += drawVar(data, VCXXFLAGS);
  total += printNl(data->fd);
  return total;
}

static ssize_t printFile(outFileData* data, t_node* head, const char* const from) {
  ssize_t total = 0;
  char path[MAXPATHLEN * 2];
  for (t_node* tmp = head; tmp; tmp = tmp->next) {
    if (IS_FOLDER(tmp)) {
      snprintf(path, MAXPATHLEN * 2, "%s/%s", from, tmp->data.name);
      total += printFile(data, tmp->child, path);
    }
    if (IS_CPP(tmp))
      data->cpp = true;
    if (IS_C_CPP(tmp))
      total += output(data->fd, "%s/%s \\\n", from, tmp->data.name);
  }
  return total;
}

static ssize_t drawFile(outFileData* data) {
  ssize_t total = 0;
  t_node* tmp = data->scb->node;
  total += output(data->fd, "file=\"\\\n");
  total += printFile(data, tmp, data->scb->originPath);
  total += output(data->fd, "\"\n\n");
  return total;
}

ssize_t buildBash(outFileData* data) {
  (void)data;
  ssize_t totalBytes = 0;
  if (!newFile("build.sh", data))
    return -1;
  const char* const hardcodePname = strrchr(data->scb->originPath, FILE_SEP) + 1;
  const char* const sh = readVariableName(data, Vshell);
  totalBytes += output(data->fd, "#!/bin/env %s\n\n", sh);
  totalBytes += header(data, findCommentFromType(data->outputType), getenv("USER"), hardcodePname, "sh");
  //
  totalBytes += drawNameAndStartVar(data);
  totalBytes += drawFile(data);
  totalBytes += drawCompiler(data);
  //
  close(data->fd);
  printf("outfile -> %s/build.sh\n", data->scb->originPath);
  return totalBytes;
}