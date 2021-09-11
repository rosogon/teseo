#ifndef actions_h
#define actions_h

#include "sensors.h"
#include "driver.h"

#define DEFAULT_VELOCITY 150

/*
 * Go is a basic go forward Action at a given speed
 * 
 * On start(), moves forward (or backward)
 * On stop(), stops.
 */
class Go : public Action {
  int _vel;
  Driver *_driver;
  bool _started;

public:
  Go(Driver &, int vel = DEFAULT_VELOCITY);
  const char *name() {
    return "Go";
  }
  void start();
  void stop();
  bool isStarted();

};

/*
 * Move allows to turn, spin or go (see Driver::move2)
 * 
 * On start(), starts movement
 * On stop(), stops.
 */
class Move : public Action {
  int _vel;
  action_T _action;
  side_T _side;
  int _delta;
  Driver *_driver;
  bool _started;

public:
  Move(Driver &, int vel, action_T action, side_T side = NONE, int delta = 0);
  const char *name() {
    return "Move";
  }
  void start();
  void stop();
  bool isStarted();
};

/*
 * Measure wall is a Loop that performs one ultrasonic measure.
 * 
 * On start(), measures.
 * On loop(), measures.
 */
class MeasureWall : public Loop {
  Sensors *_sensors;

public:
  MeasureWall(Sensors &);
  const char *name() { 
    return "MeasWall";
  }
  void start();
  void loop();
};

/*
 * Elapsed is a Condition that evaluates to true when a given amount of time has elapsed.
 */
class Elapsed : public Condition {
  Times _t;
  unsigned _duration;

public:
  Elapsed(unsigned duration);
  void start();
  bool eval();
};

/*
 * NoWall is a Condition that:
 * 
 * on start(): checks if there is a near wall
 * on eval(): returns true if the wall is not detected
 */
class NoWall : public Condition {
  Sensors *_sensors;
  side_T _sideToFollow;
  
  public:
    NoWall(Sensors &);
    const char *name() {
      return "NoWall";
    }
    void start();
    bool eval();
};

#define DETECT_ANGLE 3600
#define MAX_DRIFT 15
#define MIN_DRIFT 5

/*
 * FollowAngle is a Loop that tries to maintain the compass angle obtained on start()
 */
class FollowAngle: public Loop {
  Driver *_driver;
  Sensors *_sensors;
  int _target;
  int _origSpeed;
  bool _correcting;

  int calcDrift(double curAngle, int targetAngle);

public:
  FollowAngle(Driver &, Sensors &, int target = DETECT_ANGLE);
  const char *name() {
    return "FolAng";
  }
  void start();
  void loop();
};

/*
 * FollowWall is a Loop that maintains the distance with a given wall 
 * (the closest wall if sideToFollow is NONE)
 */
class FollowWall : public Loop {
  Driver *_driver;
  Sensors *_sensors;
  side_T _sideToFollow;
  int _target;
  
  int e0;
  int sE;
  int d0;
  unsigned long t0;
  int o0;
  double kp = 2, ki = 0.1, kd = 20;

public:
  FollowWall(Driver &, Sensors &, side_T sideToFollow = NONE, 
      double kp = 1, double ki = 0.1, double kd = 10);
  const char *name() {
    return "FolWal";
  }
  void start();
  void loop();

  inline int getError() { return e0; }
  inline int getOutput() { return o0; }
  inline int getLastDistance() { return d0; }
};

/*
 * TargetAngle is a Condition that evaluates to true when a given angle is reached
 */

class TargetAngle : public Condition {
  int _target = 0;
  int _shift = 0;
  Sensors *_sensors;
  
public:
  /*
   * 
   * targetShift > 0 : clockwise rotation
   * targetShift < 0 : counter-clockwise rotation
   */
  TargetAngle(Sensors &, int targetShift = 0);
  const char *name() {
    return "TargAng";
  }
  
  void start();
  bool eval();
};


#endif
