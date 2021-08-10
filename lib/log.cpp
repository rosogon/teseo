#include "log.h"
#include <Arduino.h>
#include <stdarg.h>

#define MAXLINE 100

//#define DISABLE_LOG 1

Print *console = &Serial;

void setConsole(Print &newConsole) {
  console = &newConsole;
}

void vtlog(const char *prefix, const char *format, va_list va) {
#ifndef DISABLE_LOG
  char buf[MAXLINE];

  console->print(millis());
  console->print(" - ");
  if (prefix && *prefix) {
    console->print(prefix);
  }
  vsnprintf(buf, MAXLINE, format, va);
  console->println(buf);
#endif
}

void tlog(const char *prefix, const char *format, ...) {
#ifndef DISABLE_LOG

  va_list va;
  va_start(va, format);
  vtlog(prefix, format, va);
  va_end(va);
#endif
}
