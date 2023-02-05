#ifndef WONDERER_LOGGER_H
#define WONDERER_LOGGER_H

typedef enum {
  LEVEL_INFO = 0,
  LEVEL_WARNING,
  LEVEL_ERROR,
  LEVEL_NONE
} WondererLogLevel;

#define WondererLog(...)                                                       \
  wonderer_logger_out(LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define WondererWarning(...)                                                       \
  wonderer_logger_out(LEVEL_WARNING, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define WondererError(...)                                                       \
  wonderer_logger_out(LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define WondererGLWrap(func)                                                   \
  wonderer_gl_clear_errors();                                                  \
  func;									\
  wonderer_gl_print_errors(__FILE__, __LINE__, __func__)

void wonderer_logger_out(WondererLogLevel level, const char *file, int line, const char *func,
                         const char *message, ...);

void wondererSetLogLevel(WondererLogLevel level);
WondererLogLevel wondererGetLogLevel();

void wondererInitGLLogger();

void wonderer_gl_clear_errors();
void wonderer_gl_print_errors(const char *file, int line, const char *func);

#endif 
