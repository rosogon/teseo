#ifndef ultrasonic_h
#define ultrasonic_h

#include <Arduino.h>
#ifdef __AVR__
#include <MePort.h>
#else
#include "arduinomock.h"
#endif

static const int MAX_CM = 400;

class UltrasonicSensor : public MePort {

private:
  volatile long _lastEnterTime;
  volatile int _measureValue;
  uint8_t _trigger, _echo;
  uint16_t _maxCm;
  unsigned long _maxTime;
  unsigned long _trigger2Time;
  unsigned long _startTime;     /* start of echo going HIGH */
  int _state = LOW;             /* state of echo pulse */
  
public:
  UltrasonicSensor(uint8_t port, uint16_t maxCm = MAX_CM);
  void setPins(uint8_t triggerPin, uint8_t echoPin);
  int distanceCm();
  void trigger();
  void trigger1();
  bool trigger2();
  int checkEcho();
};


class UltrasonicSet {
  
public:
  UltrasonicSet(UltrasonicSensor* set[], size_t n, bool parallel);
  ~UltrasonicSet();
  int *distanceCm();
  size_t size();
  
  /*
   * Type of parallelization:
   *   == 1 : trigger --> checkEcho
   *   != 1 : trigger1 --> trigger2 --> checkEcho
   * 
   * != 2 has not proved better performance and code is more complex. Default to parallelType = 1.
   */
  int parallelType = 1;
  
private:
  UltrasonicSensor** _set;
  size_t _n;
  bool _parallel;
  
  int* _distance;
  bool* _waitTrigger;

  int* distanceSerial();
  int* distanceParallel();
};


#endif
