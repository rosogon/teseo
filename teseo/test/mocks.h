#pragma once
#include <hardware.h>
#include <ultrasonic.h>

class UltrasonicSensorMock: public UltrasonicSensor {

  int _nextVal;

public:
  UltrasonicSensorMock(): UltrasonicSensor(PORT_1) {
  }

  void nextValue(int val) {
    _nextVal = val;
  }

  int distanceCm() {
    return _nextVal;
  }
};

