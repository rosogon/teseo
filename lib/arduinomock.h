/*
 * Contains mocking code not provided by arduino-ci. 
 * It also contains mocks for makeblock's library code.
 */
 
#ifndef arduinomock_h
#define arduinomock_h

#ifndef __AVR__


/* Makeblock defines */
#define NC 0

#define PORT_1  (0x01)
#define PORT_2  (0x02)
#define PORT_3  (0x03)
#define PORT_4  (0x04)
#define PORT_5  (0x05)
#define PORT_6  (0x06)
#define PORT_7  (0x07)
#define PORT_8  (0x08)
#define PORT_9  (0x09)
#define PORT_10 (0x0a)
#define M1      (0x09)
#define M2      (0x0a)
#define PORT_11 (0x0b)
#define PORT_12 (0x0c)
#define PORT_13 (0x0d)
#define PORT_14 (0x0e)
#define PORT_15 (0x0f)
#define PORT_16 (0x10)


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
  uint8_t pin1();
  uint8_t pin2();

protected:
  uint8_t s1;
  uint8_t s2;
  uint8_t _port;
  uint8_t _slot;

};

class MeCompass : public MePort {

public:
  MeCompass(uint8_t port);
  double getAngle();
};

class MeDCMotor : public MePort {
public:
  MeDCMotor(uint8_t port);
  void run(int16_t speed);
  int16_t speed() { return last_speed; }
private:
  int16_t last_speed;
};

extern MePort_Sig mePort[17];

unsigned long pulseIn(int pin, int value, unsigned long timeout = 1 * 1000 * 1000);

#endif

#endif
