#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>

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

void logging_init() {
  mkdir("logs", 0777);
  f = fopen("logs/latest.log", "w+");
  if (!f) {
    perror("ERROR! Cannot open logs/latest.log file");
    exit(0);
  }
}

void logging_deinit() {
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
