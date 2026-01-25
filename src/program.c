# include "program.h"
# include <dirent.h>
# include <sys/stat.h>

void printfolder(t_FilesList* list, int tab, int mode) {
  const char* b = "\t\t\t\t\t\t\t\t\t\t\t\t\t";
  while (list) {
    if (strncmp(".", list->data.name, 2) != 0 && strncmp("..", list->data.name, 3) != 0) {
      if (mode && list->data.type != -1)
        printf("%.*s[%d]%s p -> %p\n", tab, b, list->data.type, list->data.name, list->parant);
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

static void setup(programParam* data, char* wd, t_setting* setting) {
  memset(data, 0, sizeof(*data));
  data->setting = setting;
  if (data->setting->workignDir[0]) {
    const size_t l = strlen(data->setting->workignDir) + 1;
    memcpy(wd, data->setting->workignDir, l);
  }
  else {
    getcwd(wd, PATH_MAX);
    printf("current folder %s\n", wd);
  }
}

int build(t_setting* setting, t_FilesList* root);

int program(t_setting* setting) {
  int error = 0;
  programParam data;
  char wd[PATH_MAX + 1];
  //
  setup(&data, wd, setting);
  //
  t_FilesList* list = NULL;
  error = mapingDir(wd, &list, 0);
  printfolder(list, 0, 1);
  build(setting, list);
  freeNode(&list);
  return error;
}