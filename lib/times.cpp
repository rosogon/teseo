#include <Arduino.h>
#include "times.h"

Times::Times() {
}

void Times::start(unsigned span_ms) {
  _span = span_ms;
  reset(millis());
}

long Times::isLap() {
  _n++;
  
  unsigned long now = millis();
  if (now < _until) {
    return -1;
  }
  
  long result = _n;
  reset(now);
  return result;
}


void Times::reset(unsigned long now) {
    _until = now + _span;
    _n = 0;
}
