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

static void* dell_node(t_node* n) {
  free(n->data.name);
  free(n);
  return NULL;
}

void printfolder(t_node* list, int tab, int mode) {
  const char* b = "|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t";
  while (list) {
    if (strncmp(".", list->data.name, 2) != 0 && strncmp("..", list->data.name, 3) != 0) {
      //test_co(list, list->next);
      if (list->data.type != folder)
        printf("%.*s[%s]%s\n", tab * 2, b, getFileTypeSimble(list->data.type), list->data.name);
      else {
        printf("%.*s[%s][%zu]%s\n", tab * 2, b, getFileTypeSimble(list->data.type), list->data.fsize, list->data.name);
      }
    }
    if (list->child) {
      printfolder(list->child, tab + 1, mode);
    }
    list = list->next;
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


static bool testDotsFiles(const char* name) {
  return (strncmp(".", name, 2) == 0 || strncmp("..", name, 3) == 0);
}


// not null safe
static bool isEmtyF(t_node* node) {
  return (node->data.type == folder && node->data.fsize == 0);
}



int deledEmty(t_node** list) {
  if (!list && !*list) {
    return 0;
  }
  int dell = 0;
  HEADDELL: // first time using it
  while (*list && isEmtyF(*list)) {
    t_node* tmp = (*list)->next;
    t_node* tooFree = *list;
    dell_node(tooFree);
    *list = tmp;
    dell++;
  }
  t_node* tmp = *list;
  while (tmp) {
    if (tmp->next && isEmtyF(tmp->next)) {
      t_node* tooFree = tmp->next;
      t_node* next = tooFree->next;
      tmp->next = next;
      dell_node(tooFree);
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
  if (isValidFolder(path) || !maxDep)
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


static void setup(t_SCB* setting) {
  getcwd(setting->path, PATH_MAX);
  memcpy(setting->configPath, ".", 2);
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

int setStart(void* in) {
  t_setting* ptr = in;
  (void)ptr;
  t_SCB  SCB;
  //
  setup(&SCB);
  //
  t_node* list = NULL;
  SCB.error = mapDir(SCB.path, &list, 10);
  if (!SCB.error) {
    //move_folder_up(&list, 0);
    //connectToFolder(list, NULL);
    //deledEmty(&list);
    moveFolderUp(&list);
    deledEmty(&list);
    printfolder(list, 0, 1);
  }
  freeNode(&list);
  return SCB.error;
}