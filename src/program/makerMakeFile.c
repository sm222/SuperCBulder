# include "MakerUtilse.h"

# include <ctype.h>

static char* capName(const char* name) {
  static char newName[MAXPATHLEN + 20];
  size_t len = strlen(name);
  bzero(newName, MAXPATHLEN + 20);
  while (len--) {
    newName[len] = (char)toupper(name[len]);
  }
  return newName;
}


static ssize_t drawVarName(t_node* tmp, const char* from, const int* fd) {
  if (from)
    return output(*fd, "F_%zu_%s\t\t=\t\t$(F_%s)%s/\n\n", tmp->data.id, capName(tmp->data.name), from ,tmp->data.name);
  return output(*fd, "F_%zu_%s\t\t=\t\t%s/\n\n", tmp->data.id, capName(tmp->data.name), tmp->data.name);
}

static ssize_t drawVar(outFileData* data, const int name, const char* defaultValue) {
  const char* val = defaultValue;
  if (isVarInConfig(name, data->var)) {
    val =  readVariableName(data, reserveVarName[name]);
  }
  return output(data->fd, "%s\t\t=\t\t%s\n", reserveVarName[name], val);
}

static bool isNextFileValid(t_node* node) {
  while (node && node->next) {
    if (IS_C_CPP(node->next)) {
      return true;
    }
    node = node->next;
  }
  return false;
}

static ssize_t drawFile(t_node* n, const char* name, const int fd) {
  const bool valid = isNextFileValid(n);
  return output(fd, "\t$(F_%s)%s%s\n", name, n->data.name, valid ? "\\" : "");
}

ssize_t putAllFiles(outFileData* data, t_node* tmp, const char* from, const int fd) {
  makeOutVarLast(from, &data->outVar);
  ssize_t t = output(fd, "O_%s\t=\t\\\n", from);
  while (tmp) {
    if (IS_CPP(tmp)) {
      data->cpp = true;
    }
    if (IS_C_CPP(tmp) && !IS_FOLDER(tmp))
      t += drawFile(tmp, from, fd);
    tmp = tmp->next;
  }
  t += output(fd, "\n");
  return t;
}

void extractVar(const char* l, size_t start, size_t *end) {
  size_t i = 0;
  while (l[start + i]) {
    if (l[start + i] == ':') {
      break ;
    }
    i++;
  }
  *end = i;
}

static bool testIsIgnore(const char* name, const char* list) {
  if (!list)
    return false;
  size_t start = 0;
  size_t end = 0;
  while (list[start]) {
    extractVar(list, start, &end);
    if (strncmp(list + start, name, end) == 0)
      return true;
    start += end + 1;
  }
  return false;
}

static ssize_t  buidFileAndFolder(outFileData* data, t_node** head, const char* from, const int* fd) {
  t_node* tmp = *head;
  ssize_t t = 0;
  char folderName[MAXPATHLEN];
  bzero(folderName, MAXPATHLEN);
  const char* ignore = NULL;
  if (isVarInConfig(Ving, data->var)) {
    ignore = readVariableName(data, "ING");
  }
  while (tmp) {
    // edit that to make var unique
    if (IS_FOLDER(tmp) && !testIsIgnore(tmp->data.name, ignore)) {
      snprintf(folderName, MAXPATHLEN, "%zu_%s", tmp->data.id, capName(tmp->data.name));
      t += drawVarName(tmp, from ,fd);
      t += buidFileAndFolder(data, &tmp->child, folderName, fd);
      t += write(*fd, "\n", 1);
    }
    else if (IS_FILE(tmp)) {
      t += putAllFiles(data, tmp, from, *fd);
      return t;
    }
    tmp = tmp->next;
  }
  return t;
}


static ssize_t printRoot(const char* root, outFileData* data) {
  return output(data->fd, "F_%s\t\t=\t\t%s/\n", root, data->workingDirectory);
}

static ssize_t readList(t_node** head, outFileData* data) {
  ssize_t t = 0;
  const int fd = data->fd;
  const char* truckPath = strrchr(data->workingDirectory, '/');
  //! switch that line for windows or other case that path don't have a /
  t += printRoot(truckPath + 1, data);
  //
  char folderName[MAXPATHLEN];
  snprintf(folderName, MAXPATHLEN, "%s", truckPath + 1);
  t += buidFileAndFolder(data, head, folderName, &fd);
  return t;
}

ssize_t drawName(const char* name, outFileData* data) {
  ssize_t t = 0;
  t += drawVar(data, Vname, name);
  t += drawVar(data, Vnamex, "");
  t += output(data->fd, "\n\n");
  return t;
}


static ssize_t drawCompiler(outFileData* data) {
  ssize_t total = 0;
  total += drawVar(data, Vcc, "cc");
  total += drawVar(data, Vcxx, "c++");
  total += output(data->fd, "\n\rDEBUG\t\t\t=\t\t-g\n\n");
  total += drawVar(data, VCFLAGS, "-Wall -Werror -Wextra $(DEBUG)");
  total += drawVar(data, VCXXFLAGS, "-Wall -Werror -Wextra $(DEBUG)");
  total += output(data->fd, "\n\n\n");
  return total;
}

static ssize_t drawObjectVar(outFileData* data) {
  ssize_t t = output(data->fd, "SRCS_FILES\t\t=\t\\\n");
  t_outVar* tmp = data->outVar;
  while (tmp) {
    t += output(data->fd, "\t$(O_%s)%s\n", tmp->name, tmp->next ? "\t\\" : "");
    tmp = tmp->next;
  }
  t += output(data->fd, "\nOBJS	=	$(SRCS_FILES:.c=.o)\n\n");
  return t;
}

static ssize_t drawMakeRule(outFileData* data) {
  ssize_t t = 0;
  t += output(data->fd, "#is cpp: %s\n\n", data->cpp ? "yes" : "no");
  t += output(data->fd, "all: ");
  //! add dependece
  t += output(data->fd, "$(NAME)\n\n");
  const char* compiler = data->cpp ? "CXX" : "CC";
  t += output(data->fd, "$(NAME): $(%s)\n\n", compiler);
  t += output(data->fd, "$(%s): $(OBJS)\n\t$(%s) $(CFLAGS) $(OBJS)", compiler, compiler);
  //!add more var if needed
  t +=  output(data->fd, " -o $(NAME)$(NAMEX)\n\n");
  return t;
}

static ssize_t drawEnd(outFileData* data) {
  ssize_t t = 0;
  t += output(data->fd, "clean:\n\t@rm -fv $(OBJS)\n\n");
  t += output(data->fd, "fclean: clean\n\t@rm -fv $(NAME)\n\n");
  t += output(data->fd, "re: fclean all\n\nPHONY:\n#END");
  return t;
}


ssize_t buildMakefile(outFileData* data) {
  ssize_t totalBytes = 0;
  // rework later
  const char* hardcodePname = strrchr(data->scb->originPath, '/') + 1;
  if (!newFile("Makefile", data))
    return -1;
  totalBytes += header(data->fd, findCommentFromType(data->outputType), getenv("USER"), hardcodePname, "Makefile");
  totalBytes += output(data->fd, "# %d\n", data->fd);
  totalBytes += drawCompiler(data);
  totalBytes += drawName(hardcodePname, data);
  //
  totalBytes += readList(&data->scb->node, data);
  totalBytes += drawObjectVar(data);
  totalBytes += drawMakeRule(data);
  totalBytes += drawEnd(data);
  freeOutVar(&data->outVar);
  return totalBytes;
}

