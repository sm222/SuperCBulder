#include "MakerUtilse.h"
#include "makerMakeFile.h"
#include "makerBash.h"

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

inline short printNl(const int fd) {
  return write(fd, "\n", 1);
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

size_t header(outFileData* data, const char* comment, const char* uName, const char* pName, const char* fType) {
  time_t rawtime;
  struct tm* timeinfo;
  time(&rawtime);
  const char* maker = uName ? uName : "unknown";
  timeinfo = localtime(&rawtime);
  size_t out = 0;
  out += output(data->fd, "%s - %s - %s\n", comment, comment, comment);
  out += output(data->fd, "%s %s Make whit scb on %s",comment, fType, asctime(timeinfo));
  out += output(data->fd, "%s build by %s\n", comment, maker);
  out += output(data->fd, "%s project name -> %s\n", comment, pName);
  out += output(data->fd, "%s config file -> %s\n", comment, data->configFile.name);
  out += output(data->fd, "%s - %s - %s\n", comment, comment, comment);
  out += output(data->fd, "\n\n");
  return out;
}

#include <ctype.h>



outFileData makerSetup(t_SCB* in, int mode) {
  outFileData data;
  bzero(&data, sizeof(data));
  data.var.size = (sizeof(reserveVarName) / sizeof(char*)) - 1;
  data.scb = in;
  data.outputType = mode;
  data.workingDirectory = in->path;
  data.target = SYSTYPE;
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
  const char* name = strrchr(data->scb->path, FILE_SEP) + 1;
  output(fd, "NAME:%s\n", name);
  output(fd, "NAMEX:\n\n");
  output(fd, "# ignore folder, exp: 3part\n");
  output(fd, "ING:\n\n");
  output(fd, "# add var from config file\n");
  output(fd, "PROG:\n\n");
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
  const size_t size = (MAXPATHLEN * 2) + 2;
  char  path[size];
  snprintf(path, size,"%s/%s", data->scb->originPath, data->configFile.name);
  data->configFile.fd = open(path, O_RDONLY);
  fprintf(stderr, "path -> %s\n", path);
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
      free(data->configFile.rawData[i]);
      i++;
    }
    free(data->configFile.rawData);
    data->configFile.rawData = NULL;
    return 0;
  }
  return 1;
}

void printVar(outFileData* data) {
  for (size_t i = 0; i < data->var.size; i++) {
    printf("%s[%c]\n", reserveVarName[i], data->var.varVAlue[i] ? 'x' : ' ');
  }
}

int removeEndlP(char* value, size_t p) {
  if (!value)
    return 0;
  if (value[p] == '\n') {
    value[p] = 0;
    return 1;
  }
  return 0;
}

int removeEndl(char* value) {
  if (!value)
    return 0;
  size_t l = strlen(value) - 1;
  if (value[l] == '\n') {
    value[l] = 0;
    return 1;
  }
  return 0;
}

void extractVar(const char* l, size_t start, size_t *end, char const sep) {
  size_t i = 0;
  while (l[start + i]) {
    if (l[start + i] == sep) {
      break ;
    }
    i++;
  }
  *end = i;
}

static int isVarName(char* str, const char* const* var) {
  int i = 0;
  for (; var[i]; i++) {}
  const int size = i;
  for (i = 0; i < size; i++) {
    size_t varLen = strlen(var[i]);
    if (strncmp(str, var[i], varLen) == 0 && str[varLen] == ':') {
      return i;
    }
  }
  return -1;
}

static int checkVar(outFileData* data) {
  if (!data->configFile.fd)
    return 0;
  for (size_t i = 0; data->configFile.rawData[i]; i++) {
    const int res = isVarName(data->configFile.rawData[i], reserveVarName);
    if (res >= 0 && !data->var.varVAlue[res]) {
      data->var.varVAlue[res] = true;
    }
    else if (res >= 0) {
      fprintf(stderr, "scb: %s redeclare var at ->\n", reserveVarName[res]);
      fprintf(stderr, "%s:%zu\n",data->configFile.name, i + 1);
      return 2;
    }
    const bool* array = data->var.varVAlue;
    if (array[Vprog] + array[Vlib]  + array[Vdlib] > 1) {
      fprintf(stderr, MULT_COMPILE_RULE);
      return 3;
    }
  }
  return 0;
}

static bool isVar(const char* line, const char* varName, size_t l) {
  return (strncmp(line, varName, l) == 0 && line[l] == ':');
}

ssize_t addTo(char* to, const char* line, ssize_t* curentLen) {
  const size_t len = strlen(line);
  if (MAX_VAR_NAME_LEN - *curentLen + len <= 0) {
    fprintf(stderr, "scb: var is too long\n");
    return 0;
  }
  memcpy(to + *curentLen, line, len);
  *curentLen += len;
  return len;
}

ssize_t addToc(char* to, char c, size_t curentLen) {
  if (MAX_VAR_NAME_LEN - curentLen <= 0) {
    fprintf(stderr, "scb: var is too long\n");
    return 0;
  }
  to[curentLen] = c;
  return 1;
}

size_t skipWhiteSpace(const char* s, size_t start) {
  size_t i = 0;
  if (!s || *s == '\n')
    return 0;
  while (isspace(s[i + start])) {
    i++;
  }
  return i;
}


static inline ssize_t findVarLen(const char* s) {
  ssize_t i = 0;
  while (s[i] && !isspace(s[i]) && isalnum(s[i])) { i++; }
  return i;
}

static void getParsingError(int type) {
  if (type < 0)
    type *= -1;
  const char* line = "";
  if (type == L_invalid || type == L_unknown) {
    line = "line is invalid\n";
  }
  else if (type == L_var) { line = "invalid variable\n";}
  else if (type == L_varValue) { line = "floting value\n"; }
  fprintf(stderr,"%s\n", line);
}

static inline bool valid(int prev, int now) {
  if (now == L_invalid || now == L_unknown)
    return false;
  if (now == L_varValue) {
    if (prev == L_var || prev == L_varValue) {
      return true;
    }
    return false;
  }
  return now >= 0 ? true : false;
}

static int isLineValid(const char* s) {
  int lineType = s ? L_unknown : L_empty;
  if (s) {
    if (*s == '#' || *s == '\n') { return L_comment; }
    else if (isdigit(*s) || *s == ':' ) { return L_invalid; }
    else if (isspace(*s))  { lineType = L_varValue; }
    else if (isalpha(*s))  { lineType = L_var; }
    else if (!isalpha(*s)) { return  L_invalid; }
    size_t i = 0;
    while (s[i]) {
      if ((!isspace(s[i]) && lineType == L_varValue) || \
      (s[i] == ':' && lineType == L_var)) {
        return lineType;
      }
      if (isspace(s[i]) && lineType == L_var)
        break ;
      i++;
    }
    lineType *= -1; //! flip value to show error
  }
  return lineType;
}


int checkIfFileValid(outFileData* data) {
  if (!data->configFile.fd)
    return 0;
  size_t i = 0;
  int prev = L_empty;
  while (data->configFile.rawData[i]) {
    const char* l = data->configFile.rawData[i];
    const int line = isLineValid(l);
    if (!valid(prev, line)) {
      //! add better error message
      fprintf(stderr, "scb: invalid config file\n" \
        "%s:%zu - > %s\n", data->configFile.name, i + 1, l);
      getParsingError(line);
      return 1;
    }
    i++;
    prev = line;
  }
  return 0;
}

static void readEnv(outFileData* data, const char* s, ssize_t* total) {
  size_t i = 0;
  if (!data->scb->mainData->env)
    return ;
  const char* const* env = data->scb->mainData->env;
  s += 4;
  const size_t varLen = findVarLen(s);
  while (env[i]) {
    if (strncmp(env[i], s, varLen) == 0 && env[i][varLen] == '=') {
      addTo(data->configFile.buffer, env[i] + varLen + TOKENSIZE, total);
    }
    i++;
  }
}

static int testKeyWord(outFileData* data, const char* s, size_t* dis, ssize_t* total) {
  const size_t len = findVarLen(s);
  *dis += len;
  short i = 0;
  for ( ; keyWords[i]; i++) {
    const size_t lenKeyword = strlen(keyWords[i]);
    if (strncmp(s, keyWords[i], lenKeyword) == 0) { break ; }
  }
  // test system target
  if (i <= NUMBER_OF_OS - 1) {
    return !(data->target == i);
  }
  if (i == k_env) {
    readEnv(data, s, total);
    return 0;
  }
  if (i == k_shell) {
    //data->shellFt(data, total, start, s);
    fprintf(stderr, "shell!!\n");
  }
  return 1;
}

//! add suport for default value
static bool IsKnowVar(outFileData* data, ssize_t* total, const size_t varlen, const char* name) {
  size_t i = 0;
  while (reserveVarName[i]) {
    if (strncmp(name, reserveVarName[i], varlen) == 0) {
      if (isVarInConfig(i, data->var)) {
        return false;
      }
      else {
        addTo(data->configFile.buffer, reserveVarNameDefaultValue[i], total);
        return true;
      }
    }
    i++;
  }
  return false;
}


static size_t getValue(outFileData* data, ssize_t* total, const size_t start, const char* name) {
  if (*total >= MAX_VAR_NAME_LEN)
    return 0;
  size_t i = 0;
  const char* l = NULL;
  const size_t nameLen = findVarLen(name);
  if (IsKnowVar(data, total, nameLen, name))
    return nameLen;
  while (data->configFile.rawData[i]) {
    l = data->configFile.rawData[i];
    if (i > start) { return nameLen; }
    if (strncmp(l, name, nameLen) == 0 && l[nameLen] == ':') { break ; }
    i++;
    if (!data->configFile.rawData[i]) { return nameLen; }
  }
  size_t j = nameLen + 1;
  bool nlValid = false;
  do {
    const size_t lineLen = strlen(l);
    nlValid = false;
    while (j < lineLen) {
      if (l[j] == '\\' && l[j + 1] == '%') {
        addToc(data->configFile.buffer, '%', (*total)++);
        j += 2;
      }
      else if (l[j] == '%') {
        if (l[j + 1] == '_') {
          if (testKeyWord(data, l + j + (TOKENSIZE * 2), &j, total))
            break ;
        }
        j += getValue(data, total, i, l + j + TOKENSIZE) + TOKENSIZE;
      }
      else {
        addToc(data->configFile.buffer, l[j], (*total)++);
        j++;
      }
    }
    (*total) -= removeEndl(data->configFile.buffer);
    l = data->configFile.rawData[++i];
    if (isLineValid(l) == L_varValue) {
      j = skipWhiteSpace(l, 0);
      nlValid = true;
    }
  } while (nlValid);
  return nameLen;
}


//* return value from file or default value
char* readVariableName(outFileData* data, e_reserveVarName name) {
  bzero(data->configFile.buffer, MAX_VAR_NAME_LEN);
  size_t i = 0;
  ssize_t curentLen = 0;
  const size_t len = strlen(reserveVarName[name]);
  if (isVarInConfig(name, data->var)) {
    while (data->configFile.rawData[i]) {
      if (isVar(data->configFile.rawData[i], reserveVarName[name], len)) {
        getValue(data, &curentLen, i, reserveVarName[name]);
        break;
      }
      i++;
    }
  }
  else {
    //* safe because default value don't have token
    const size_t valueLen = strlen(reserveVarNameDefaultValue[name]);
    memcpy(data->configFile.buffer, reserveVarNameDefaultValue[name], valueLen);
  }
  return data->configFile.buffer;
}


inline int isVarInConfig(int var, t_reserveVar varList) {
  return varList.varVAlue[var];
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
    printf("config file found\n");
    data->configFile.name = getConfigName(data, 1);
  }
  else {
    const char* r = dialogBox(WITCH_FILE, "[number]", 2);
    const int n = atoi(r);
    data->configFile.name = getConfigName(data, n);
  }
  openConfigFile(data);
  error += checkIfFileValid(data);
  error += checkVar(data);
  //! printVar(data); !add flag for showing it
  if (data->outputType == makefile && !error) {
    outB = buildMakefile(data);
  } else if (data->outputType == sh && !error) {
    outB = buildBash(data);
  } else {
    const char* msg = "";
    if (error) {  msg = ERROR_FILE; }
    else { msg = UNKNOWN_TYPE; }
    fprintf(stderr, msg, data->outputType);
  }
  closeConfigFile(data);
  printf("total byte prints > %zu\n", outB);
  return error;
}


