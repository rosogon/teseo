#include "sensors.h"
#include <log.h>

#define SENSORS_DEBUG 1

#include <stdarg.h>

static void _log(const char *format, ...) {
    LOG_VA("SEN - ", format);
}

Sensors::Sensors(MeCompass *compass, int delayCompass, 
                UltrasonicSet *ultrasonicSet, int delayUltrasonic) {
                  
  this->compass = compass;
  this->usSet = ultrasonicSet;
  timesCompass.start(delayCompass);
  timesUltrasonic.start(delayUltrasonic);
  timesCycles.start(1000);
}

void Sensors::loop(byte sensors) {
  bool idle = true;
  if ( (sensors & COMPASS) && timesCompass.isLap() != -1) {
    angle = compass->getAngle();
    _compass_loops++;
    idle = false;
    
    #ifdef SENSORS_DEBUG
    _log("angle=%d", (int)angle);
    #endif
  }

  if ( (sensors & ULTRASONIC) && timesUltrasonic.isLap() != -1) {
    distance = usSet->distanceCm();
    _us_loops++;
    idle = false;
    
    #ifdef SENSORS_DEBUG
    char buf[100] = "";
    char *ptr = buf;
    for (size_t i = 0; i < usSet->size(); i++) {
      int size = snprintf(ptr, 100 - strlen(buf), "[%lu]=%i ", i, distance[i]);
      ptr += size;
    }
    _log("%s", buf);
    #endif
  }

  if (idle) {
    _idle_loops++;
  }
  int cycles = timesCycles.isLap();
  if (cycles != -1) {
    _log("STATS - loops/s=%d us=%d compass=%d idle=%d", cycles, _us_loops, _compass_loops, _idle_loops);
    resetCounters();
  }
}

void Sensors::resetCounters() {
  _idle_loops = 0;
  _us_loops = 0;
  _compass_loops = 0;
}

double Sensors::getAngle() {
  return angle;
}

int* Sensors::getDistance() {
  return distance;
}
