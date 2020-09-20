#ifndef serialreader_h
#define serialreader_h

#include <Arduino.h>

#define BUFSIZE 80

class SerialReader {
  char* _buf;
  size_t _i;
  size_t _size;

public:
  SerialReader(size_t size = BUFSIZE);
  /*
   * Reads characters into buf until \r or \n is found. Do not read 
   * 
   * Returns:
   * 0: no more available characters and EOL not found
   * > 0 (= strlen(buf)): EOL found; buf contains the line (without EOL characters); 
   * < 0 (= -strlen(buf)): Buffer is filled; next invocation overwrites  
   */
  int read();

  /*
   * Securely returns the read string. It is ensured it is a line if previous call to
   * read() returned 0.
   */
  char* string();
};

#endif
