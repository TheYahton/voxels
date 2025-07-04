#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

#include "logging.h"

#ifdef _WIN32
#define lmkdir(dir) mkdir(dir)
#else
#define lmkdir(dir) mkdir(dir, 0755)
#endif // _WIN32

#define DEBUG_FORMAT "[%s] [DEBUG]: "
#define INFO_FORMAT "[%s] [INFO]: "
#define WARNING_FORMAT "[%s] [WARNING]: "
#define ERROR_FORMAT "[%s] [ERROR]: "

const char *LOGGING_MESSAGES[] = {
  DEBUG_FORMAT,
  INFO_FORMAT,
  WARNING_FORMAT,
  ERROR_FORMAT,
};

static void formatted_time(char *buffer) {
  time_t timer = time(NULL);
  struct tm *tm_info = localtime(&timer);

  strftime(buffer, 26, "%H:%M:%S", tm_info);
}

FILE *f;
enum LoggingLevel LOGGING_LEVEL = LL_DEBUG;

bool logging_init(enum LoggingLevel l) {
  LOGGING_LEVEL = l;

  lmkdir("logs");
  f = fopen("logs/latest.log", "w+");
  if (!f) {
    perror("ERROR! Cannot open logs/latest.log file");
    return 0;
  }

  return 1;
}

void logging_deinit(void) {
  fclose(f);
}

void logging_log(enum LoggingLevel ll, const char *fmt, ...) {
  if (ll < LOGGING_LEVEL) return;
  char buffer[26];
  formatted_time(buffer);

  va_list args;

  va_start(args, fmt);
  fprintf(f, LOGGING_MESSAGES[ll], buffer);
  vfprintf(f, fmt, args);
  fprintf(f, "\n");
  va_end(args);

  va_start(args, fmt);
  printf(LOGGING_MESSAGES[ll], buffer);
  vprintf(fmt, args);
  printf("\n");

  va_end(args);
}
