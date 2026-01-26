# include "../dataType.h"
# include "../utilse.h"
# include "node.h"
# include <dirent.h>
# include <sys/stat.h>
# include "fileType.h"

bool test_co(t_FilesList* l1, t_FilesList* l2) {
  if (l1 == l2) {
    printf("loop!!!!\n");
    return false;
  }
  
  if (l1 && l2) {
    bool t[2] = {false, false};
    if (l1->next == l2)
      t[0] = true;
    if (l2->prev == l1)
      t[1] = true;
    printf("[%c][%c]%s %s\n", t[0] ? 'x' : ' ', t[0] ? 'x' : ' ', l1 ? l1->data.name : "", l2 ? l2->data.name : "");
    return true;
  }
  return false;
}

void printfolder(t_FilesList* list, int tab, int mode) {
  //const char* b = "_____________________________________";
  while (list) {
    usleep(60000);
    if (strncmp(".", list->data.name, 2) != 0 && strncmp("..", list->data.name, 3) != 0) {
      test_co(list, list->next);
      //if (mode && list->data.type != -1)
      //  printf("%.*s[%d]%s %s\n", tab, b, list->data.type, list->data.name, list->parant ? "yes" : "");
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

size_t get_len_list(t_FilesList* list) {
  if (!list)
    return 0;
  size_t i = 0;
  for (; list; list = list->prev) { ; }
  for (; list; list = list->next) { i++; }
  return i;
}

static void move_folder_up(t_FilesList** list, int d) {
  if (!get_to_start(list))
    return;
  int i = 0;
  t_FilesList* tmp = *list;
  const size_t len = get_len_list(*list);
  printf("%zu\n", len);
  for ( ; tmp->next; tmp = tmp->next) {
    //if (tmp->data.type == folder) {
    //  move_folder_up(&tmp->child, d + 1);
    //  printf("here?\n");
    //}
    (void)d;
    if (tmp->data.type > tmp->next->data.type) {
      t_FilesList* tmp1;
      t_FilesList* tmp2;
      t_FilesList* swap1;
      t_FilesList* swap2;
      printf("swap\n");
      tmp1 = tmp->prev;       //1
      swap1 = tmp;            //2
      swap2 = tmp->next;      //3
      tmp2 = tmp->next->next; //4
      if (tmp1) {
        tmp1->next = swap2;
        swap2->prev = tmp1;
      }
      if (tmp2) {
        tmp2->prev = swap1;
        swap1->next = tmp2;
      }
      swap2->next = swap1;
      swap1->prev = swap2;
    }
    i++;
    if (i > 50)
      break;
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
    if (strcmp(fileType[i], cut) == 0)
      return i;
  }
  return -1;
}

# define MAX_DEP 5

# include "node.h"

int mapingDir(const char* dir, t_FilesList** list, int dep) {
  if (isValidFolder(dir) || dep > MAX_DEP)
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
      int type = S_ISDIR(stats.st_mode) ? folder : unknown;
      if ((strncmp(".", de->d_name, 2) == 0 || strncmp("..", de->d_name, 3) == 0))
        continue;
      t_FilesList* t = makeNodeLast(de->d_name, type, list);
      if (type == folder) {
        mapingDir(wd, &t->child, dep + 1);
        t->data.size = getNodeLen(t->child);
      }
      else
        t->data.type = getFileType(de->d_name);
    }
  } while (de != NULL);
  closedir(dr);
  return 0;
}

static void setup(char* wd, t_setting* setting) {
  (void)setting;
  getcwd(wd, PATH_MAX);
  printf("current folder %s\n", wd);
}


int code(t_setting* setting) {
  int error = 0;
  char wd[PATH_MAX + 1];
  //
  setup(wd, setting);
  //
  t_FilesList* list = NULL;
  error = mapingDir(wd, &list, 0);
  printfolder(list, 0, 1);
  move_folder_up(&list, 0);
  printfolder(list, 0, 1);
  freeNode(&list);
  return error;
}

int setStart(void* in) {
  t_setting* ptr = in;
  
  return code(ptr);
}