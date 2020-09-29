#ifndef log_h
#define log_h

#include <Arduino.h>

//template <typename ... Args> void tlog(const char *prefix, const char *format, Args... args);

void tlog(const char *prefix, const char *format, ...);

/*
 * setConsole allows to set a different Print object, instead of Serial
 * (useful for testing)
 */
void setConsole(Print &newConsole);

#endif
