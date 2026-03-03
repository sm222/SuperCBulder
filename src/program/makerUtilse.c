#include "MakerUtilse.h"

const size_t bSize = 9999;


static t_outVar* makeOutVar(const char* name) {
  t_outVar* out = calloc(1, sizeof(*out));
  if (!out)
    return NULL;
  out->name = strdup(name);
  return out;
}

t_outVar* makeOutVarLast(const char* name, t_outVar** list) {
  if (!list || !name)
    return NULL;
  if (!(*list)) {
    *list = makeOutVar(name);
    if (!(*list)) {
      return NULL;
    }
    return *list;
  }
  t_outVar* tmp = *list;
  while (tmp && tmp->next) {
    tmp = tmp->next;
  }
  tmp->next = makeOutVar(name);
  return tmp->next;
}

void freeOutVar(t_outVar** list) {
  if (!list)
    return;
  t_outVar* tmp = *list;
  t_outVar* next = NULL;
  while (tmp) {
    next = tmp->next;
    free(tmp->name);
    free(tmp);
    tmp = next;
  }
  *list = NULL;
}

void printOutVar(t_outVar* head) {
  size_t i = 0;
  while (head) {
    printf("[%zu]%s\n", i++, head->name);
    head = head->next;
  }
}

# define NL '\n'
# define B_SIZE 101

#  define BUFFER_SIZE 100

typedef struct s_info {
  char*  tmp;
  char*  tmp2;
  char   readtmp[BUFFER_SIZE + 1];
  int    rv;
  size_t  cut;
}  t_info;

static void* ft_sfree(void* ptr) {
  free(ptr);
  return NULL;
}

static char* ft_strjoin(char* sfree, char* s2) {
  size_t s1_i = strlen(sfree ? sfree : "");
  size_t s2_i = strlen(s2 ? s2 : "");
  char* new = calloc(s1_i + s2_i + 1, sizeof(char));

  if (!new) {
    sfree = ft_sfree(sfree);
    return (NULL);
  }
  while (s1_i + s2_i-- > s1_i)
    new[s1_i + s2_i] = s2[s2_i];
  while (s1_i--)
    new[s1_i] = sfree[s1_i];
  if (*new == 0)
    new = ft_sfree(new);
  return (ft_sfree(sfree), new);
}

static char* ft_tiny_split(char* s, size_t* cut) {
  char* new = NULL;
  size_t  i = 0;

  while (s[i])
    if (s[i++] == '\n')
      break ;
  new = calloc(i + 1, sizeof(char));
  if (!new)
    return (new = ft_sfree(new));
  *cut = i;
  while (i--)
    new[i] = s[i];
  return (new);
}

static char  ft_find_nl(char* s) {
  size_t  i = 0;

  while (s && s[i]) {
    if (s[i] == '\n')
      return ('\n');
    i++;
  }
  return ('0');
}

static char* safe_return(char**  book, t_info* t_val) {
  t_val->tmp = ft_tiny_split(*book, &t_val->cut);
  if (!t_val->tmp) {
    *book = ft_sfree(*book);
    return (NULL);
  }
  t_val->tmp2 = *book;
  *book = ft_strjoin(NULL, *book + t_val->cut);
  t_val->tmp2 = ft_sfree(t_val->tmp2);
  return (t_val->tmp);
}

char* get_next_line(int fd) {
  static char* book = NULL;
  t_info    t_val;

  if (fd < 0 || BUFFER_SIZE <= 0 || fd > 255)
    return (book = ft_sfree(book));
  if (!book)
    book = calloc(1, sizeof(char));
  if (!book)
    return (NULL);
  t_val.rv = 0;
  while (ft_find_nl(book) == '0') {
    bzero(t_val.readtmp, BUFFER_SIZE + 1);
    t_val.rv = read(fd, t_val.readtmp, BUFFER_SIZE);
    if (t_val.rv <= 0)
      break ;
    book = ft_strjoin(book, t_val.readtmp);
    if (!book)
      return (NULL);
  }
  if (t_val.rv == -1 || (t_val.rv <= 0 && *book == 0)) {
    book = ft_sfree(book);
    return (NULL);
  }
  return (safe_return(&book, &t_val));
}

size_t output(int fd, const char * s, ...) {
  char buffer[bSize + 1];
  va_list list;
  va_start(list, s);
  const int len = vsnprintf(buffer, bSize, s, list);
  if (len > 0)
    write(fd, buffer, len);
  va_end(list);
  return len;
}


#include <time.h>

size_t header(int fd, const char* comment, const char* uName, const char* pName, const char* fType) {
  time_t rawtime;
  struct tm* timeinfo;
  time(&rawtime);
  const char* maker = uName ? uName : "unknown";
  timeinfo = localtime(&rawtime);
  size_t out = 0;
  out += output(fd, "%s - %s - %s\n", comment, comment, comment);
  out += output(fd, "%s %s Make whit scb on %s",comment, fType, asctime(timeinfo));
  out += output(fd, "%s build by %s\n", comment, maker);
  out += output(fd, "%s project name -> %s\n", comment, pName);
  out += output(fd, "%s - %s - %s\n", comment, comment, comment);
  out += output(fd, "\n\n");
  return out;
}

#include <ctype.h>

int superStrcmp(const char* s1, const char* s2, size_t n) {
  if(!s1 || !s2 || n == 0)
    return -1;
  while (*s1 && *s2 && tolower(*s1) == tolower(*s2) && --n) {
    s1++;
    s2++;
  }
  return *s1 - *s2;
}


outFileData makerSetup(t_SCB* in, int mode) {
  outFileData data;
  bzero(&data, sizeof(data));
  data.scb = in;
  data.outputType = mode;
  data.workingDirectory = in->path;
  memcpy(data.cCompiler,   "cc", 3);
  memcpy(data.cppCompiler, "c++", 4);
  memcpy(data.configFilename, "", 1);
  return data;
}

# include "makerMakeFile.h"

char* dialogBox(const char* question, const char* option, unsigned int reposeSize) {
  //fprintf(stderr, "no config file found, do you want to continue?\n");
  //fprintf(stderr, "[y] yes | [m] make one | anyting else no\n");
  fprintf(stderr, "%s\t\n", question);
  fprintf(stderr, "%s\t\n", option);
  if (reposeSize == 0)
    return 0;
  static char out[1001];
  ssize_t l = read(STDIN_FILENO, out, reposeSize); //! add safety
  out[l] = 0;
  return out;
}


size_t findDot(const char* s) {
  size_t i = 0;
  while (s[i]) {
    if (s[i] == ':')
      return i;
    i++;
  }
  return 0;
}

int printConfigFiles(t_node* head) {
  char buff[PATH_MAX + 1];
  memset(buff, '_', PATH_MAX);
  buff[PATH_MAX] = 0;
  t_node* tmp = head;
  size_t maxLen = 0;
  int nbConfigFile = 0;
  for (; tmp; tmp = tmp->next) {
    if (IS_SCB(tmp)) {
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
      if (tmp->data.type == configFile) {
        printf("[%i]%s\n", ++index,tmp->data.name);
      }
    }
    printf("%.*s\n", (int)maxLen + 4, buff);
  }
  return nbConfigFile;
}

static char* getConfigName(outFileData* data, unsigned int i) {
  if (!data || i == 0) return NULL;
  t_node* t = data->scb->node;
  unsigned int j = 0;
  while (t) {
    if (IS_SCB(t)) {
      j++;
      if (j == i) {
        return t->data.name;
      }
    }
    t = t->next;
  }
  return NULL;
}

bool newFile(char *name, outFileData *data) {
  if (!name || !data)
    return false;
  data->fd = open(name, O_CREAT | O_TRUNC | O_RDWR, 0644);
  if (!data->fd)
    return false;
  return true;
}

static bool makeDefaultConfigFile(outFileData* data) {
  const int fd = open("defaultConfigFile.scb", O_CREAT | O_TRUNC | O_RDWR, 0644);
  if (!fd) {
    fprintf(stderr, "scb: fail to make default file %s\n", strerror(errno));
    return false;
  }
  //! add safety later
  const char* name = strrchr(data->scb->path, '/') + 1;
  output(fd, "NAME:%s\n", name);
  output(fd, "NAMEX:\n\n");
  output(fd, "MAXDEPH:5\n\n");
  output(fd, "# ignore folder, exp: var/\n");
  output(fd, "ING:\n\n");
  output(fd, "# add var from config file\n");
  output(fd, "CR:\n\n");
  output(fd, "# dependecy\n");
  output(fd, "DEP:\n\n");
  output(fd, "\n# all those variable are reserve\n");
  for (int i = 0; reserveVarName[i]; i++) {
    output(fd, "# %s:\n", reserveVarName[i]);
  }
  output(fd, "#\n");
  close(fd);
  printf("default file made\n");
  return true;
}

static int makeChoiseNoConfigFile(char respose, outFileData* data) {
  if (respose == 'c') {
    return 0;
  }
  else if (respose == 'm') {
    makeDefaultConfigFile(data);
  }
  return 1;
}

char* findCommentFromType(int type) {
  if (type >= 0) {
    return strchr(buildFileLanguage[type], ':') + 1;
  }
  return "";
}

static int readConfigFile(t_configValue* file) {
  size_t alloc = 20;
  size_t index = 0;
  char* l = "";
  file->rawData = calloc(alloc, sizeof(char*));
  while (l) {
    if (alloc / 2 == index) {
      alloc *= 2;
      file->rawData = realloc(file->rawData, sizeof(char*) * alloc);
    }
    file->rawData[index] = get_next_line(file->fd);
    l = file->rawData[index];
    index++;
  }
  return 0;
}


static int openConfigFile(outFileData* data) {
  if (!data->configFile.name) {
    return 1;
  }
  data->configFile.fd = open(data->configFile.name, O_RDONLY);
  if (data->configFile.fd == 0) {
    perror("scb");
    return 1;
  }
  readConfigFile(&data->configFile);
  return 0;
}

static int closeConfigFile(outFileData* data) {
  if (data->configFile.fd) {
    close(data->configFile.fd);
    size_t i = 0;
    while (data->configFile.rawData[i]) {
      printf("%s\n" , data->configFile.rawData[i]);
      free(data->configFile.rawData[i]);
      i++;
    }
    free(data->configFile.rawData);
    data->configFile.rawData = NULL;
    return 0;
  }
  return 1;
}


int makerStart(outFileData* data) {
  ssize_t outB = 0;
  int error = 0;
  const int configFile = printConfigFiles(data->scb->node);
  if (!configFile) {
    const char* r =  \
    dialogBox(NO_CONFIG_FILE, WITCH_FILE_QUESTION, 1);
    if (makeChoiseNoConfigFile(r[0], data)) {
      fprintf(stderr, "scb: stop\n");
      return 1;
    }
  } else if (configFile == 1) {
    printf("config file foud\n");
    data->configFile.name = getConfigName(data, 1);
  }
  else {
    const char* r = dialogBox(WITCH_FILE, "[number]", 2);
    const int n = atoi(r);
    data->configFile.name = getConfigName(data, n);
  }
  openConfigFile(data);
  if (data->outputType == makefile && !error) {
    outB = buildMakefile(data);
  }
  closeConfigFile(data);
  printf("total byte prints > %zu\n", outB);
  return error;
}


