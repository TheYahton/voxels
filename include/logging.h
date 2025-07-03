#ifndef LOGGING_H
#define LOGGING_H

enum LoggingLevel {
  LL_DEBUG = 0,
  LL_INFO,
  LL_WARN,
  LL_ERROR,
};

bool logging_init(enum LoggingLevel min);
void logging_deinit(void);
void logging_log(enum LoggingLevel ll, const char *fmt, ...);

#endif // LOGGING_H
