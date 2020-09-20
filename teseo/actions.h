#ifndef actions_h
#define actions_h

#include "sensors.h"
#include "driver.h"

#define DEFAULT_VELOCITY 150

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

class Elapsed : public Condition {
  Times _t;
  unsigned _duration;

public:
  Elapsed(unsigned duration);
  void start();
  bool eval();
};

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

class FollowWall : public Loop {
  Driver *_driver;
  Sensors *_sensors;
  side_T _sideToFollow;
  int _target;

public:
  FollowWall(Driver &, Sensors &, side_T sideToFollow = NONE);
  const char *name() {
    return "FolWal";
  }
  void start();
  void loop();
};

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
