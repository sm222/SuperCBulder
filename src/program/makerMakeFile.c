# include "MakerUtilse.h"

# include <ctype.h>

char* capName(const char* name) {
  static char newName[MAXPATHLEN + 20];
  size_t len = strlen(name);
  bzero(newName, MAXPATHLEN + 20);
  while (len--) {
    newName[len] = (char)toupper(name[len]);
  }
  return newName;
}

static ssize_t drawVarName(const char* name, const char* from, const int* fd) {
  if (from)
    return output(*fd, "F_%s\t\t=\t\t$(F_%s)%s/\n\n", capName(name), from ,name);
  return output(*fd, "F_%s\t\t=\t\t%s/\n\n", capName(name) ,name);
}

ssize_t drawFile(t_node* n, const char* name, const int fd) {
  return output(fd, "\t$(F_%s)%s%s\n", name, n->data.name, n->next ? "\\" : "");
}

//todo draw file in a object var

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

static ssize_t  buidFileAndFolder(outFileData* data, t_node** head, const char* from, const int* fd) {
  t_node* tmp = *head;
  ssize_t t = 0;
  char folderName[MAXPATHLEN];
  bzero(folderName, MAXPATHLEN);
  if (tmp && IS_FOLDER(tmp)) {
    memcpy(folderName, capName(tmp->data.name), strlen(tmp->data.name) + 1);
    printf("%s/%s|\n", from, folderName);
  }
  while (tmp) {
    // edit that to make var unique
    if (IS_FOLDER(tmp)) {
      t += drawVarName(tmp->data.name, from ,fd);
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


ssize_t printRoot(const char* root, outFileData* data) {
  return output(data->fd, "F_%s\t\t=\t\t%s/\n", root, data->workingDirectory);
}

static ssize_t readList(t_node** head, outFileData* data) {
  ssize_t t = 0;
  const int fd = data->fd;
  const char* truckPath = strrchr(data->workingDirectory, '/');
  //! switch that line for windows or other case that path don't hold a /
  t += printRoot(truckPath + 1, data);
  //
  t += buidFileAndFolder(data ,head, truckPath + 1, &fd);
  return t;
}

ssize_t drawName(const char* name, const int fd) {
  return output(fd, "NAME\t\t=\t\t%s\n\n", name);
}

static ssize_t drawCompiler(outFileData* data) {
  ssize_t total = 0;
  total += output(data->fd, "CC\t\t=\t%s\n", data->cCompiler);
  total += output(data->fd, "CXX\t\t=\t%s\n", data->cppCompiler);
  total += output(data->fd, "\n\rDEBUG\t\t\t=\t\t-g\n\n", data->cCompiler);
  total += output(data->fd, "CFLAGS\t\t\t=\t-Wall -Werror -Wextra $(DEBUG)\n", data->cCompiler);
  total += output(data->fd, "CCFLAGS\t\t=\t-Wall -Werror -Wextra $(DEBUG)\n\n\n", data->cCompiler);
  return total;
}

static ssize_t drawObjectVar(outFileData* data) {
  ssize_t t = output(data->fd, "SRCS_FILES\t\t=\t\\\n");
  t_outVar* tmp = data->outVar;
  while (tmp) {
    t += output(data->fd, "\t$(O_%s)\t\t\\\n", tmp->name);
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
  t +=  output(data->fd, " -o $(NAME)\n\n");
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
  if (data->configFd) {
    //parsingFile(data);
  }
  totalBytes += header(data->fd, findCommentFromType(data->outputType), getenv("USER"), hardcodePname, "Makefile");
  totalBytes += drawCompiler(data);
  totalBytes += drawName(hardcodePname, data->fd);
  totalBytes += readList(&data->scb->node, data);
  totalBytes += drawObjectVar(data);
  totalBytes += drawMakeRule(data);
  totalBytes += drawEnd(data);
  freeOutVar(&data->outVar);
  return totalBytes;
}


int printConfigFiles(t_node* head) {
  char buff[PATH_MAX + 1];
  memset(buff, '_', PATH_MAX);
  buff[PATH_MAX] = 0;
  t_node* tmp = head;
  size_t maxLen = 0;
  size_t nbConfigFile = 0;
  for (; tmp; tmp = tmp->next) {
    if (tmp->data.type == configFile) {
      nbConfigFile++;
      const size_t l = strlen(tmp->data.name);
      maxLen < l ? maxLen = l: maxLen;
    }
  }
  if (nbConfigFile) {
    tmp = head;
    int index = 0;
    printf("%.*s\n", (int)maxLen + 4, buff);
    for (; tmp; tmp = tmp->next) {
      if (tmp->data.type == configFile)
        printf("[%i]%s\n", ++index,tmp->data.name); 
    }
    printf("%.*s\n", (int)maxLen + 4, buff);
  }
  return 0;
}