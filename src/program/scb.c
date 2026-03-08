#include "scb.h"

static char* getFileTypeSimble(int n) {
  if (n == folder)
    return "📁";
  if (n == cFile)
    return "C";
  if (n == hFile)
    return "H";
  if (n == cppFile)
    return "CPP";
  if (n == hppFile)
    return "HPP";
  if (n == tppFile)
    return "TPP";
  if (n == configFile)
    return "SCB";
  return "error!";
}


void printfolder(t_node* list, int tab, int mode) {
  const char* b = "|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  \
|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |";
  char buff[10];
  buff[0] = 0;
  while (list) {
    if (mode) {
      bzero(buff, 10);
      snprintf(buff, 10, "[%zu]", list->data.id);
    }
    if (strncmp(".", list->data.name, 2) != 0 && strncmp("..", list->data.name, 3) != 0) {
      if (list->data.type != folder)
        printf("%.*s[%s]%s%s\n", tab * 3, b, getFileTypeSimble(list->data.type), buff, list->data.name);
      else {
        printf("%.*s[%s]%s[%zu]%s\n", tab * 3, b, getFileTypeSimble(list->data.type), buff, list->data.fsize, list->data.name);
      }
    }
    if (list->child) {
      printfolder(list->child, tab + 1, mode);
    }
    list = list->next;
  }
}



static int testFolderList(const char* folder) {
  const size_t i = getArrayLen(DefaultIgnoreFolder);
  size_t j = 0;
  while (j < i) {
    if (strcmp(folder, DefaultIgnoreFolder[j]) == 0)
      return 1;
    j++;
  }
  return 0;
}


static int isValidFolder(const char* path) {
  if (path) {
    const char* dirTruck = strrchr(path, '/');
    if (dirTruck && testFolderList(dirTruck)) {
      fprintf(stdout, "%s is ignore\n", dirTruck + 1);
      return 1;
    }
  }
  return 0;
}

static int getFileType(const char* fileName) {
  if (!fileName)
    return -1;
  const char* cut = strrchr(fileName, '.');
  if (!cut)
    return -1;
  for (int i = 0; fileType[i]; i++) {
    if (strcmp(fileType[i], cut) == 0) {
      return i;
    }
  }
  return -1;
}


static bool testDotsFiles(const char* name) {
  return (strncmp(".", name, 2) == 0 || strncmp("..", name, 3) == 0);
}


int deledEmty(t_node** list) {
  if (!list && !*list) {
    return 0;
  }
  int dell = 0;
  HEADDELL: // first time using it
  while (*list && IS_FOLDER_EMPTY(((t_node*)*list))) {
    t_node* tmp = (*list)->next;
    t_node* tooFree = *list;
    dellNode(tooFree);
    *list = tmp;
    dell++;
  }
  t_node* tmp = *list;
  while (tmp) {
    if (tmp->next && IS_FOLDER_EMPTY(tmp->next)) {
      t_node* tooFree = tmp->next;
      t_node* next = tooFree->next;
      tmp->next = next;
      dellNode(tooFree);
      tmp = *list;
      dell++;
      continue ;
    }
    if (tmp->data.type == folder) {
      tmp->data.fsize -= deledEmty(&tmp->child);
      if (tmp->data.fsize == 0) {
        goto HEADDELL;
      }
    }
    tmp = tmp->next;
  }
  return dell;
}


int mapDir(const char* path, t_node** head, unsigned int maxDep) {
  if (isValidFolder(path) || maxDep == 0)
    return 1; // only care if error happen of first try
  struct dirent* de = NULL;
  DIR* dr = opendir(path);
  if (dr == NULL) {
    fprintf(stderr, "scb: can't open or read %s\n", path);
    return 1;
  }
  struct stat stats;
  char wd[PATH_MAX + 1];
  do {
    de = readdir(dr);
    if (de) {
      snprintf(wd, PATH_MAX, "%s/%s", path, de->d_name);
      stat(wd, &stats);
      const int type = S_ISDIR(stats.st_mode) ? folder : getFileType(de->d_name);
      if (testDotsFiles(de->d_name) || (type == unknown && type != folder) || isValidFolder(wd)) {
        continue ;
      } else {
        t_node* t = makeNodeLast(de->d_name, type, head);
        if (type == folder) {
          mapDir(wd, &t->child, --maxDep);
          t->data.fsize = getNodeLen(t->child);
        }
      }
    }
  } while (de != NULL);
  closedir(dr);
  return 0;
}


static bool isSwap(t_node* n) {
  if (n && n->next) {
    if (n->data.type != folder && n->next->data.type == folder)
      return true;
  }
  return false;
}

static void swapData(t_node* n) {
  t_node_data tmp = n->data;
  n->data = n->next->data;
  n->next->data = tmp;
  n->child = n->next->child;
  n->next->child = NULL;
}

void moveFolderUp(t_node** list) {
  t_node* tmp = *list;
  while (tmp) {
    if (isSwap(tmp)) {
      swapData(tmp);
      tmp = *list;
      continue ;
    }
    if (tmp->data.type == folder) {
      moveFolderUp(&tmp->child);
    }
    tmp = tmp->next;
  }
}

# include "MakerUtilse.h"

static int setup(t_SCB* setting, void* mainData) {
  bzero(setting, sizeof(*setting));
  setting->mainData = mainData;
  getcwd(setting->originPath, PATH_MAX);
  const size_t avNb = av_len(&setting->mainData->avNoFlags);
  if (!avNb) {
    fprintf(stderr, "scb: no path given\n");
    return 1;
  }
  const char* path = av_read(&setting->mainData->avNoFlags, 0);
  if (chdir(path)) {
    fprintf(stderr, "scb: invalid path: %s\n",path);
    return 1;
  }
  if (avNb > 1) {
    setting->buildType = av_read(&setting->mainData->avNoFlags, 1);
  }
  getcwd(setting->path, PATH_MAX);
  return 0;
}

int scb(void* data) {
  t_SCB  SCB;
  if (setup(&SCB, data)) {
    return 1;
  }
  //
  SCB.error = mapDir(SCB.path, &SCB.node, 30);
  chdir(SCB.originPath);
  if (!SCB.error) {
    moveFolderUp(&SCB.node);
    deledEmty(&SCB.node);
    //! add flag for visual
    printfolder(SCB.node, 0, 0);
    outFileData data = makerSetup(&SCB, 0);
    SCB.error = makerStart(&data);
  }
  freeNode(&SCB.node);
  return SCB.error;
}


int setStart(void* in) {
  t_setting* ptr = in;
  ptr->programFt = &scb;
  return 0;
}