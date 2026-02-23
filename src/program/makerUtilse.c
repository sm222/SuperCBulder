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

static char* get_next_line(int fd) {
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

int read_file(outFileData* file) {
  const size_t startSize = 10;
  file->var = calloc(startSize, sizeof(char*));
  size_t numberLine = 0;
  size_t rawFileSize = startSize;
  char* line = "line";
  while (line) {
    line = get_next_line(file->configFd);
    file->var[numberLine] = line;
    file->varByte += strlen(line ? line : "");
    numberLine++;
    if (numberLine > rawFileSize / 2) {
      rawFileSize *= 2;
      file->var = realloc(file->var, sizeof(char*) * rawFileSize);
    }
  }
  file->varArray = numberLine;
  return 0;
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

static void makeConfig(void) {
  const int fd = open("config.scb", O_CREAT | O_TRUNC , 0644);
  if (fd < 0) {
    perror("scb");
  }
  close(fd);
}

char* dialogBox(const char* question, const char* option, unsigned int reposeSize) {
  //fprintf(stderr, "no config file found, do you want to continue?\n");
  //fprintf(stderr, "[y] yes | [m] make one | anyting else no\n");
  fprintf(stderr, "%s\t\n", question);
  fprintf(stderr, "%s\t\n", option);
  if (reposeSize == 0)
    return 0;
  static char out[1001];
  ssize_t l = read(STDIN_FILENO, out, reposeSize);
  out[l] = 0;
  return out;
}

int testConfigFile(outFileData* data) {
  char buff[MAXPATHLEN * 2];
  sprintf(buff, "%s/%s", data->workingDirectory, data->configFilename);
  if (access(buff, R_OK) == 0) {
    data->configFd = open(buff, O_RDONLY);
    if (data->configFd < 0) {
      perror("open");
      return 1;
    }
    read_file(data);
    return 0;
  }
  perror(data->configFilename);
  const char* respose = dialogBox("A", "B", 1);
  if (respose[0] == 'y')
    return 0;
  if (respose[0] == 'm') {
    makeConfig();
    return 1;
  }
  fprintf(stderr, "scb: stop\n");
  return 1;
}

static size_t findDot(const char* s) {
  size_t i = 0;
  while (s[i]) {
    if (s[i] == ':')
      return i;
    i++;
  }
  return 0;
}


int getFileType(outFileData* data) {
  if (data->var && data->var[0]) {
    size_t i = 0;
    while (buildFileLanguage[i]) {
      if (strncmp(data->var[0], "TYPE:", 5) == 0) {
        size_t dotP = findDot(buildFileLanguage[i]);
        if (superStrcmp(data->var[0] + 5, buildFileLanguage[i], dotP) == 0 \
        && ENDL(*(data->var[0] + 5 + dotP))) {
          data->outputType = i;
          printf("type chose - %s\n", buildFileLanguage[i]);
          return 0;
        }
      }
      i++;
    }
    fprintf(stderr, "scb: bad headder\n%s\n", data->var[0]);
    return 1;
  } else {
    fprintf(stderr, "scb: bad headder\n");
    return 1;
  }
  return 0;
}


int makerStart(outFileData* data) {
  ssize_t outB = 0;
  int error = 0;
  //if (!data || testConfigFile(data))
  //  return 1;
  //if (data->configFd) {
  //  error = getFileType(data);
  //}
  //lookForConfigFile(data);
  if (data->outputType == makefile && !error) {
    outB = buildMakefile(data);
  }
  printf("total byte prints > %zu\n", outB);
  closeFile(data);
  return error;
}

bool newFile(char *name, outFileData *data) {
  if (!name || !data)
    return false;
  data->fd = open(name, O_CREAT | O_TRUNC | O_RDWR, 0644);
  if (!data->fd)
    return false;
  return true;
}

void closeFile(outFileData *data) {
  close(data->fd);
  if (data->configFd) {
    close(data->configFd);
    size_t i = 0;
    while (data->var[i]) {
      free(data->var[i]);
      i++;
    }
    free(data->var);
  }
}

char* findCommentFromType(int type) {
  if (type >= 0) {
    return strchr(buildFileLanguage[type], ':') + 1;
  }
  return "";
}
