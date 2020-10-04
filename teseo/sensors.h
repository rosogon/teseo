#ifndef sensors_h
#define sensors_h

#include <ultrasonic.h>
#include <hardware.h>
#include <times.h>

#define COMPASS 1
#define ULTRASONIC 2


class Sensors {

public:
  Sensors(MeCompass *compass, int delayCompass, UltrasonicSet *ultrasonicSet, int delayUltrasonic);
  void loop(byte sensors);
  double getAngle();
  int* getDistance();
  
private:
  MeCompass *compass;
  UltrasonicSet *usSet;
  Times timesCycles;
  Times timesCompass;
  Times timesUltrasonic;
  double angle = 0;
  int *distance;

  int _idle_loops = 0;    // idle loops per second
  int _us_loops = 0;      // ultrasonic measures per second
  int _compass_loops = 0; // compass measures per second

  void resetCounters();
};

#endif
