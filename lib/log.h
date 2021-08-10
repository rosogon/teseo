#ifndef log_h
#define log_h

#include <Arduino.h>

#define LOG_VA(prefix, format) { \
    va_list va; \
    va_start(va, format); \
    vtlog(prefix, format, va); \
    va_end(va); \
}

void vtlog(const char *prefix, const char *format, va_list va);
void tlog(const char *prefix, const char *format, ...);

/*
 * setConsole allows to set a different Print object, instead of Serial
 * (useful for testing)
 */
void setConsole(Print &newConsole);

#ifdef ARDUINO_CI

class StderrPrint: public Print {

  size_t write(uint8_t c) {
    fprintf(stderr, "%c", c);
    return 1;
  }
};

#endif

#endif
