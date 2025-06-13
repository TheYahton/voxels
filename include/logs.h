#include <stdbool.h>

enum LoggingLevel {
  LL_DEBUG = 0,
  LL_INFO,
  LL_WARN,
  LL_ERROR,
};

bool logging_init(void);
void logging_deinit(void);
void logging_log(enum LoggingLevel ll, const char *text);

/*
void info(const char *text);
void warning(const char *text);
void error(const char *text);
*/
