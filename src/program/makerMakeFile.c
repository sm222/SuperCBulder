# include "MakerUtils.h"

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

static int getNumberTab(const char* s) {
  int l = 11 - strlen(s);
  return l / 2;
}

static ssize_t drawVarName(t_node* tmp, const char* from, const int* fd) {
  if (from)
    return output(*fd, "F_%zu_%s\t\t=\t\t$(F_%s)%s/\n\n", tmp->data.id, capName(tmp->data.name), from ,tmp->data.name);
  return output(*fd, "F_%zu_%s\t\t=\t\t%s/\n\n", tmp->data.id, capName(tmp->data.name), tmp->data.name);
}

static ssize_t drawVar(outFileData* data, const int name) {
  const char* val =  readVariableName(data, name);
  const int tabSize = getNumberTab(reservedVarNames[name]);
  const char* tabs = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
  return output(data->fd, "%s%.*s=\t\t%s\n",reservedVarNames[name], tabSize, tabs, val);
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
    if (IS_C_CPP(tmp))
      t += drawFile(tmp, from, fd);
    tmp = tmp->next;
  }
  t += printNl(fd);
  return t;
}


static bool testIsIgnore(const char* name, const char* list) {
  if (!list)
    return false;
  size_t start = 0;
  size_t end = 0;
  while (list[start]) {
    extractVar(list, start, &end, ';');
    if (strncmp(list + start, name, end) == 0) {
      return true;
    }
    start += end + TOKENSIZE;
  }
  return false;
}

static ssize_t  buidFileAndFolder(outFileData* data, t_node** head, const char* from, const int* fd) {
  t_node* tmp = *head;
  ssize_t t = 0;
  char folderName[MAXPATHLEN * 2];
  const char* ignore = NULL;
  if (isVarInConfig(Ving, data->var)) {
    ignore = readVariableName(data, Ving);
  }
  while (tmp) {
    // edit that to make var unique
    if (IS_FOLDER(tmp) && !testIsIgnore(tmp->data.name, ignore)) {
      snprintf(folderName, MAXPATHLEN * 2, "%zu_%s", tmp->data.id, capName(tmp->data.name));
      t += drawVarName(tmp, from ,fd);
      t += buidFileAndFolder(data, &tmp->child, folderName, fd);
      t += printNl(*fd);
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
  const char* truckPath = strrchr(data->workingDirectory, FILE_SEP);
  //! switch that line for windows or other case that path don't have a /
  t += printRoot(truckPath + 1, data);
  //
  char folderName[MAXPATHLEN * 2];
  snprintf(folderName, MAXPATHLEN * 2, "%s", truckPath + 1);
  t += buidFileAndFolder(data, head, folderName, &fd);
  return t;
}

ssize_t drawName(const char* name, outFileData* data) {
  ssize_t t = 0;
  if (isVarInConfig(Vname, data->var))
    t += drawVar(data, Vname);
  else
    t += output(data->fd, "NAME\t\t=\t\t%s\n", name);
  //
  t += drawVar(data, Vnamex);
  t += output(data->fd, "\n\n");
  return t;
}


static ssize_t drawCompiler(outFileData* data) {
  ssize_t total = 0;
  total += drawVar(data, Vcc);
  total += drawVar(data, Vcxx);
  total += drawVar(data, VCFLAGS);
  total += drawVar(data, VCXXFLAGS);
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
  t += output(data->fd, "\n#ty https://stackoverflow.com/questions/22499486/how-to-replace-c-and-cpp-file-together");
  t += output(data->fd, "\nOBJS	=	$(addsuffix .o,$(basename $(SRCS_FILES)))\n\n");
  return t;
}

static ssize_t drawDep(outFileData* data) {
  const char* depValue = readVariableName(data, Vdep);
  ssize_t t = 0;
  t += output(data->fd, "dep:\n");
  if (!depValue)
    return t;
  size_t start = 0;
  size_t end = 0;
  while (depValue[start]) {
    extractVar(depValue, start, &end, ';');
    t += output(data->fd, "\t%.*s\n", (int)end, depValue + start);
    start += end + TOKENSIZE;
  }
  t += printNl(data->fd);
  return t;
}

static ssize_t drawProg(outFileData* data, const char* const compiler, const bool prog) {
  ssize_t t = 0;
  t += output(data->fd, "$(NAME): $(%s)\n\n", compiler);
  t += output(data->fd, "$(%s): $(OBJS)\n\t$(%s) $(CFLAGS) $(OBJS)", compiler, compiler);
  if (prog) {
    const char* const progVar = readVariableName(data, Vprog);
    t += output(data->fd, " %s ", progVar);
  }
  t += output(data->fd, " -o $(NAME)$(NAMEX)\n\n");
  return t;
}

static ssize_t drawlib(outFileData* data, const char* const ar) {
  ssize_t t = 0;
  t += output(data->fd, "$(NAME): $(OBJS)\n");
  t += output(data->fd, "\t%s $(NAME)$(NAMEX) $(OBJS)\n\n", ar);
  return t;
}

static ssize_t drawMakeRule(outFileData* data) {
  ssize_t t = 0;
  const bool dep  = isVarInConfig(Vdep, data->var);
  const bool prog = isVarInConfig(Vprog, data->var);
  const bool lib  = isVarInConfig(Vlib, data->var);
  const bool dlib = isVarInConfig(Vdlib, data->var);
  const char* const compiler = data->cpp ? "CXX" : "CC";
  t += output(data->fd, "#is cpp: %s\n\n", data->cpp ? "yes" : "no");
  t += output(data->fd, "all: ");
  if (dep) {
    t += output(data->fd, "dep ");
  }
  t += output(data->fd, "$(NAME)\n\n");
  if (prog || (!lib && !dlib)) {
    t += drawProg(data, compiler, prog);
  }
  if (lib) {
    //! add lib buid here for .so or .a
    t += drawlib(data, "ar rcs");
  }
  //!add more var if needed
  if (dep) {
    //* make dep rule
    t += drawDep(data);
  }
  return t;
}

static ssize_t drawEnd(outFileData* data) {
  ssize_t t = 0;
  t += output(data->fd, "clean:\n\t@rm -fv $(OBJS)\n\n");
  t += output(data->fd, "fclean: clean\n\t@rm -fv $(NAME)\n\n");
  t += output(data->fd, "re: fclean all\n\nPHONY:\n#END");
  return t;
}


//! need to test see if it works
static void shellCallFt(void* data, ssize_t* total) {
  outFileData* structCast = data;
  const char* const keyWord = "$(shell )";
  memcpy(structCast->shellEnd, ")", 2);
  ssize_t i = 0;
  while (keyWord[i] != ')') {
    addToc(structCast->configFile.buffer, keyWord[i], *total);
    (*total)++;
    i++;
  }
}

ssize_t buildMakefile(outFileData* data) {
  ssize_t totalBytes = 0;
  // rework later
  data->shellFt = shellCallFt;
  const char* hardcodePname = strrchr(data->scb->originPath, FILE_SEP) + 1;
  if (!newFile("Makefile", data))
    return -1;
  totalBytes += header(data, findCommentFromType(data->outputType), getenv("USER"), hardcodePname, "Makefile");
  totalBytes += drawCompiler(data);
  totalBytes += drawName(hardcodePname, data);
  //
  totalBytes += readList(&data->scb->node, data);
  totalBytes += drawObjectVar(data);
  totalBytes += drawMakeRule(data);
  totalBytes += drawEnd(data);
  close(data->fd);
  freeOutVar(&data->outVar);
  printf("outfile -> %s/Makefile\n", data->scb->originPath);
  return totalBytes;
}

