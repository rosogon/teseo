/*
 * Contains mocking code not provided by arduino-ci. 
 * It also contains mocks for makeblock's library code.
 */
 
#ifndef arduinomock_h
#define arduinomock_h

#ifndef __AVR__

#include <Arduino.h>

/* MAKEBLOCK CODE */
typedef struct
{
  uint8_t s1;
  uint8_t s2;
} MePort_Sig;

class MePort {
public:
  MePort(void);
  MePort(uint8_t port);
  MePort(uint8_t port, uint8_t slot);
  uint8_t getPort(void);
  uint8_t getSlot(void);
  bool dRead1(uint8_t mode = INPUT);

protected:
  uint8_t s1;
  uint8_t s2;
  uint8_t _port;
  uint8_t _slot;

};

extern MePort_Sig mePort[17];

unsigned long pulseIn(int pin, int value, unsigned long timeout = 1 * 1000 * 1000);

#endif

#endif
