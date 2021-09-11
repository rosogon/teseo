#include "ultrasonic.h"

#define TIMEOUT(m) (m) * 55 + 200
#define DISTANCE(duration) (duration) * 10 / 2 / 292

/*
 * UltrasonicSensor
 */
 
UltrasonicSensor::UltrasonicSensor(uint8_t port, uint16_t maxCm) : MePort(port) {
  _trigger = s1;
  _echo = s2;
  _maxCm = maxCm;
}

void UltrasonicSensor::setPins(uint8_t triggerPin, uint8_t echoPin) {
    _trigger = triggerPin;
    _echo = echoPin;
}

int UltrasonicSensor::distanceCm() {
  long duration;
  long timeout = TIMEOUT(_maxCm);

  long now = millis();

  if (now - _lastEnterTime <= 23) {
    duration = _measureValue; 
  } 
  else {
    _lastEnterTime = now;
    pinMode(_trigger, OUTPUT);
    digitalWrite(_trigger, LOW);
    delayMicroseconds(4);
    digitalWrite(_trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigger, LOW);
    pinMode(_echo, INPUT);
    duration = pulseIn(_echo, HIGH, timeout);
    _measureValue = duration;
  }

  if (duration == 0) {
    return _maxCm;
  }

  return DISTANCE(duration);
}

void UltrasonicSensor::trigger() {
  /* TODO: check lastEnterTime */
  
  long timeout = TIMEOUT(_maxCm);
  
  pinMode(_trigger, OUTPUT);
  digitalWrite(_trigger, LOW);
  delayMicroseconds(4);
  digitalWrite(_trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigger, LOW);    
  pinMode(_echo, INPUT);
  _startTime = micros();
  _maxTime = _startTime + timeout;
  _state = LOW;
}

int UltrasonicSensor::checkEcho() {
  unsigned long now = micros();
  if (now > _maxTime) {
    return _maxCm;
  }
  int in = digitalRead(_echo);

  if (_state == LOW || in == HIGH) {
    if (_state == LOW && in == HIGH) {
      _startTime = micros();
      _state = HIGH;
    }
    return 0;
  }
  
  long duration = micros() - _startTime;
  return DISTANCE(duration);
}

void UltrasonicSensor::trigger1() {
  /* TODO: check lastEnterTime */
  pinMode(_trigger, OUTPUT);
  digitalWrite(_trigger, LOW);
  delayMicroseconds(4);
  digitalWrite(_trigger, HIGH);
  _trigger2Time = micros() + 10;
}

bool UltrasonicSensor::trigger2() {
  if (micros() <= _trigger2Time) {
    return false;
  }
  digitalWrite(_trigger, LOW);    
  pinMode(_echo, INPUT);
  long timeout = TIMEOUT(_maxCm);
  _maxTime = micros() + timeout;
  _state = LOW;    
  return true;
}

/*
 * UltrasonicSet
 */

UltrasonicSet::UltrasonicSet(UltrasonicSensor* set[], size_t n, bool parallel) {
  _set = set;
  _n = n;
  _parallel = parallel;
  _waitTrigger = (bool *) calloc(n, sizeof(int));
  _distance = (int *) calloc(n, sizeof(int));
}

UltrasonicSet::~UltrasonicSet() {
  free(_waitTrigger);
  free(_distance);
}

size_t UltrasonicSet::size() {
  return _n;
}

int *UltrasonicSet::distanceCm() {
  return _parallel? distanceParallel() : distanceSerial();
}

int *UltrasonicSet::distanceSerial() {
  for (int i = 0; i < _n; i++) {
    if (_set[i] == nullptr) {
      continue;
    }
    _distance[i] = _set[i]->distanceCm();
  }
  return _distance;
}

int *UltrasonicSet::distanceParallel() {
  unsigned long t1 = millis();
  for (int i = 0; i < _n; i++) {
    _distance[i]= 0;
    if (parallelType == 1) {
      _set[i]->trigger();
      _waitTrigger[i] = true;
    }
    else {
      _set[i]->trigger1();
      _waitTrigger[i] = false;
    }
  }

  bool exit_ = false;
  while (!exit_) {
    exit_ = true;

    for (int i = 0; i < _n; i++) {
      if (!_waitTrigger[i]) {
        _waitTrigger[i] = _set[i]->trigger2();
        exit_ = false;
      } else if (!_distance[i]) {
        _distance[i] = _set[i]->checkEcho();
        exit_ = false;
      }
    }
  }
  return _distance;  
}
