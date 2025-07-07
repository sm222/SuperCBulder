# include "program.h"
# include <stdlib.h> // calloc

t_FilesList* makeNode(char* name, int type) {
  if (!name)
    return NULL;
  t_FilesList* node = NULL;
  node = calloc(1, sizeof(*node));
  if (!node)
    return NULL;
  node->name = strdup(name);
  node->type = type;
  return node;
}


// -1 missing name
// -2 no list provide
// -3 faild to make a new node
// else return len of list
t_FilesList* makeNodeLast(char* name, int type, t_FilesList** list) {
  if (!list) // look for list first 
    return NULL;
  if (!name)
    return NULL;
  int size = 0;
  t_FilesList* head = *list;
  t_FilesList* made = NULL;
  while (head) {
    size++;
    if (!head->next)
      break ;
    head = head->next;
  }
  // make head
  if (!size) {
    *list = makeNode(name, type);
    if (!(*list))
      return NULL;
    made = *list;
  }
  else {
    // add tale
    head->next = makeNode(name, type);
    if (!head->next)
      return NULL;
    made = head->next;
  }
  return made;
}


int freeNode(t_FilesList** list) {
  if (!list) {
    return -1;
  }
  int size = 0;
  t_FilesList* head = *list;
  while (head) {
    if (head->child) {
      size += freeNode(&head->child);
    }
    t_FilesList* next = head->next;
    free(head->name);
    free(head);
    head = next;
    size++;
  }
  *list = NULL;
  return size;
}

# include <dirent.h>
# include <sys/stat.h>
/*
  stat(head->name, &stats);
  if (S_ISDIR(stats.st_mode))
    printf("%s valid\n", head->name);
  else
    printf("%s invalid\n", head->name);
  head = head->next;
*/

void printfolder(t_FilesList* list, int tab, int mode) {
  const char* b = "\t\t\t\t\t\t\t\t\t\t\t\t\t";
  while (list) {
    if (strncmp(".", list->name, 2) != 0 && strncmp("..", list->name, 3) != 0) {
      if (mode && list->type != -1)
        printf("%.*s[%d]%s\n", tab, b, list->type, list->name);
    }
    if (list->child) {
      printfolder(list->child, tab + 1, mode);
    }
    list = list->next;
  }
}

int testFolderList(const char* folder) {
  size_t i = getArrayLen(ignoreFolder);
  size_t j = 0;
  while (j < i) {
    if (strcmp(folder, ignoreFolder[j]) == 0)
      return 1;
    j++;
  }
  return 0;
}

int isValidFolder(const char* path) {
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

int mapingDir(const char* dir, t_FilesList** list) {
  if (isValidFolder(dir))
    return 1;
  struct dirent* de = NULL;
  DIR* dr = opendir(dir);
  if (dr == NULL) {
    fprintf(stderr, "can't open or read %s\n", dir);
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
      t_FilesList* t = makeNodeLast(de->d_name, type, list);
      if (strncmp(".", de->d_name, 2) != 0 && strncmp("..", de->d_name, 3) != 0) {
        if (type == folder) // if foler
          mapingDir(wd, &t->child);
        else
          t->type = getFileType(de->d_name);
      }
    }
  } while (de != NULL);
  closedir(dr);
  return 0;
}

int program(t_setting* setting) {
  int error = 0;
  (void)setting;
  char wd[PATH_MAX + 1];
  getcwd(wd, PATH_MAX);
  printf("current folder %s\n", wd);
  //
  t_FilesList* list = NULL;
  mapingDir(wd, &list);
  //for (t_FilesList* head = list; head; head = head->next) {
  //  printf("> %s %d\n", head->name, head->type);
  //}
  printfolder(list, 0, 1);
  freeNode(&list);
  return error;
}