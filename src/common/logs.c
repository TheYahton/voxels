#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include "logs.h"

#ifdef _WIN32
#define lmkdir(dir) mkdir(dir)
#else
#define lmkdir(dir) mkdir(dir, 0755)
#endif // _WIN32

#define INFO_FORMAT "[%s] [INFO]: %s\n"
#define WARNING_FORMAT "[%s] [WARNING]: %s\n"
#define ERROR_FORMAT "[%s] [ERROR]: %s\n"

static void formatted_time(char *buffer) {
  time_t timer = time(NULL);
  struct tm *tm_info = localtime(&timer);

  // strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
  strftime(buffer, 26, "%H:%M:%S", tm_info);
}

FILE *f;

int logging_init(void) {
  lmkdir("logs");
  f = fopen("logs/latest.log", "w+");
  if (!f) {
    perror("ERROR! Cannot open logs/latest.log file");
    return -1;
  }
  return 0;
}

void logging_deinit(void) {
  fclose(f);
}

void info(const char *text) {
  char buffer[26];
  formatted_time(buffer);
  fprintf(f, INFO_FORMAT, buffer, text);
  printf(INFO_FORMAT, buffer, text);
}

void warning(const char *text) {
  char buffer[26];
  formatted_time(buffer);
  fprintf(f, WARNING_FORMAT, buffer, text);
  printf(WARNING_FORMAT, buffer, text);
}

void error(const char *text) {
  char buffer[26];
  formatted_time(buffer);
  fprintf(f, ERROR_FORMAT, buffer, text);
  printf(ERROR_FORMAT, buffer, text);
}
