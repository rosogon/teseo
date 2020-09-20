#include "serialreader.h"

SerialReader::SerialReader(size_t size) {
  _buf = (char *) calloc(size, sizeof(char));
  _size = size;
  _i = 0; 
}

int SerialReader::read() {
  if (Serial.available()) {
    char in = Serial.read();

    if (in == '\r' || in == '\n') {
      _buf[_i] = '\0';
      int result = _i;
      _i = 0;
      return result;     
    } else {
      _buf[_i++] = in;
    }

    if (_i == _size - 1) {
      _buf[_i] = '\0';
      _i = 0;
      return -(_size - 1);
    }
  } 
  return 0;
}

char* SerialReader::string() {
  if (_i != 0) {
    _buf[_i] = '\0';
  }
  return _buf;
}
