#include "log.h"
#include <Arduino.h>

#define MAXLINE 100

//#define DISABLE_LOG 1

void tlog(const char *prefix, const char *format, ...) {
#ifndef DISABLE_LOG
  char buf[MAXLINE];

  Serial.print(millis());
  Serial.print(" - ");
  if (prefix && *prefix) {
    Serial.print(prefix);
  }
  //snprintf(buf, MAXLINE, format, args...);
  va_list va;
  va_start(va, format);
  vsnprintf(buf, MAXLINE, format, va);
  va_end(va);
  Serial.println(buf);
#endif
}
