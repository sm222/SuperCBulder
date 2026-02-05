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
  return "error!";
}

void printfolder(t_FilesList* list, int tab, int mode) {
  const char* b = "*\t*\t*\t*\t*\t*\t*\t*\t*\t*\t*\t*\t*\t*\t*\t";
  while (list) {
    if (strncmp(".", list->data.name, 2) != 0 && strncmp("..", list->data.name, 3) != 0) {
      //test_co(list, list->next);
      if (list->data.type != folder)
        printf("%.*s[%s]%s\n", tab * 2, b, getFileTypeSimble(list->data.type), list->data.name);
      else {
        printf("%.*s[%zu][%s]%s\n", tab * 2, b, list->data.size, getFileTypeSimble(list->data.type), list->data.name);
      }
    }
    if (list->child) {
      printfolder(list->child, tab + 1, mode);
    }
    list = list->next;
  }
}

bool get_to_start(t_FilesList** list) {
  if (!list)
    return false;
  if (!*list)
    return false;
  t_FilesList* tmp = *list;
  while (tmp->prev)
    tmp = tmp->prev;
  *list = tmp;
  return true;
}

static void swapData(t_FilesList* node) {
  const FilesListData tmpdata = node->data;
  node->data = node->next->data;
  node->next->data = tmpdata;
  void* child = node->child;
  node->child = node->next->child;
  node->next->child = child;
}

static void move_folder_up(t_FilesList** list, int d) {
  if (!get_to_start(list))
    return;
  t_FilesList* tmp = *list;
  for ( ; tmp->next; tmp = tmp->next) {
    if (tmp->data.type == folder) {
      move_folder_up(&tmp->child, d + 1);
    }
    if (tmp->data.type != folder && tmp->next->data.type == folder) {
      swapData(tmp);
      tmp = *list;
    }
  }
}


static int testFolderList(const char* folder) {
  const size_t i = getArrayLen(ignoreFolder);
  size_t j = 0;
  while (j < i) {
    if (strcmp(folder, ignoreFolder[j]) == 0)
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

# define MAX_DEP 5


static bool testDotsFiles(const char* name) {
  return (strncmp(".", name, 2) == 0 || strncmp("..", name, 3) == 0);
}


static int connectToFolder(t_FilesList* list, t_FilesList* head) {
  while (list) {
    if (list->data.type == folder) {
      connectToFolder(list->child, list);
    }
    list->parant = head;
    list = list->next;
  }
  return 0;
}


int mapingDir(const char* dir, t_FilesList** list, int maxDep) {
  if (isValidFolder(dir) || !maxDep)
    return 1; // only care if error happen of first try
  struct dirent* de = NULL;
  DIR* dr = opendir(dir);
  if (dr == NULL) {
    fprintf(stderr, "scb: can't open or read %s\n", dir);
    return 1;
  }
  struct stat stats;
  char wd[PATH_MAX + 1];
  do {
    de = readdir(dr);
    if (de) {
      snprintf(wd, PATH_MAX, "%s/%s", dir, de->d_name);
      stat(wd, &stats);
      const int type = S_ISDIR(stats.st_mode) ? folder : getFileType(de->d_name);
      if (testDotsFiles(de->d_name) || (type != folder && type == unknown))
        continue;
      t_FilesList* t = makeNodeLast(de->d_name, type, list);
      if (type == folder) {
        mapingDir(wd, &t->child, --maxDep);
        t->data.size = getNodeLen(t->child);
        if (t->child && t->child->data.size == 0) {
          printf("->name:%s\n", t->child->data.name);
        }
      }
      else
        t->data.type = type;
    }
  } while (de != NULL);
  closedir(dr);
  return 0;
}

static void setup(t_SCB* setting) {
  getcwd(setting->path, PATH_MAX);
  memcpy(setting->configPath, ".", 2);
}



int setStart(void* in) {
  t_setting* ptr = in;
  (void)ptr;
  t_SCB  SCB;
  //
  setup(&SCB);
  //
  t_FilesList* list = NULL;
  SCB.error = mapingDir(SCB.path, &list, 10);
  if (!SCB.error) {
    move_folder_up(&list, 0);
    connectToFolder(list, NULL);
    printfolder(list, 0, 1);
  }
  freeNode(&list);
  return SCB.error;
}