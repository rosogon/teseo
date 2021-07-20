/*
 * Contains mocking code not provided by arduino-ci. 
 * It also contains mocks for makeblock's library code.
 */

#include "arduinomock.h"

#ifdef ARDUINO_CI


#define PIN_A0   (14)
#define PIN_A1   (15)
#define PIN_A2   (16)
#define PIN_A3   (17)
#define PIN_A4   (18)
#define PIN_A5   (19)
#define PIN_A6   (20)
#define PIN_A7   (21)

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;
static const uint8_t A4 = PIN_A4;
static const uint8_t A5 = PIN_A5;
static const uint8_t A6 = PIN_A6;
static const uint8_t A7 = PIN_A7;


MePort_Sig mePort[17] =
{
  { NC, NC }, { 11, 12 }, {  9, 10 }, { A2, A3 }, { A0, A1 },
  { NC, NC }, {  8, A6 }, { A7, 13 }, {  8, A6 }, {  6,  7 },
  {  5,  4 }, { NC, NC }, { NC, NC }, { NC, NC }, { NC, NC },
  { NC, NC },{ NC, NC },
};

MePort::MePort(uint8_t port) {
  s1 = mePort[port].s1;
  s2 = mePort[port].s2;
  _port = port;
}

uint8_t MePort::pin1() {
  return s1;
}

uint8_t MePort::pin2() {
  return s2;
}

MeCompass::MeCompass(uint8_t port) : MePort(port) {
  ;
}

double MeCompass::getAngle() {
  return 0;
}


MeDCMotor::MeDCMotor(uint8_t port) : MePort(port) {
  ;
}

void MeDCMotor::run(int16_t speed) {
  speed = speed > 255 ? 255 : speed;
  speed = speed < -255 ? -255 : speed;

  last_speed = speed;
}

#endif
