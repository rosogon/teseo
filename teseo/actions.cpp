#include "actions.h"
#include <log.h>

template <typename ... Args> static void _log(const char *format, Args... args) {
  tlog("DRI - ", format, args...);
}

Go::Go(Driver &driver, int vel) {
  this->_vel = vel;
  this->_driver = &driver;  // for some reason, if _driver is a reference, this crashes
  this->_started = false;
}

void Go::start() {
  _started = true;
  _driver->go(_vel);
}

void Go::stop() {
  _driver->stop();
}

bool Go::isStarted() {
  return _started;
}

Move::Move(Driver &driver, int vel, action_T action, side_T side, int delta) {
  _driver = &driver;
  _vel = vel;
  _action = action;
  _side = side;
  _delta = delta;
  _started = false;
}

void Move::start() {
  _started = true;
  _driver->move2(_vel, _action, _side, _delta);
}

void Move::stop() {
  _driver->stop();
}

bool Move::isStarted() {
  return _started;
}

MeasureWall::MeasureWall(Sensors &sensors) {
  _sensors = &sensors;
}

void MeasureWall::start() {
  _sensors->loop(ULTRASONIC);
}

void MeasureWall::loop() {
  _sensors->loop(ULTRASONIC);
}

Elapsed::Elapsed(unsigned duration) {
  _duration = duration;
  _t = Times();
}

void Elapsed::start() {
  _t.start(_duration);
}

bool Elapsed::eval() {
  bool done = _duration == 0 || _t.isLap() > 0;
  if (done) {
    _duration = 0;
  }
  return done;
}

#define MAX_WALL 40

NoWall::NoWall(Sensors &sensors) {
  _sensors = &sensors;
}

void NoWall::start() {
  int left = _sensors->getDistance()[SIDE2DISTANCE(LEFT)];
  int right = _sensors->getDistance()[SIDE2DISTANCE(RIGHT)];

  if (left < MAX_WALL) {
    _sideToFollow = LEFT;
  } else if (right < MAX_WALL) {
    _sideToFollow = RIGHT;
  } else {
    _sideToFollow = NONE; // Error: cannot follow any side
  }
}

bool NoWall::eval() {
  return _sensors->getDistance()[SIDE2DISTANCE(_sideToFollow)] > MAX_WALL;
}

FollowAngle::FollowAngle(Driver &driver, Sensors &sensors, int targetAngle) {
  _driver = &driver;
  _sensors = &sensors;
  _target = targetAngle;
}

void FollowAngle::start() {
  if (_target == DETECT_ANGLE) {
    _sensors->loop(COMPASS);
    _target = (int)_sensors->getAngle();
  }
  /*
   * Both speeds should be equal, but just in case;
   * if the robot is spinning, this will stop it.
   */
  _origSpeed = (_driver->left() + _driver->right()) / 2;

  //_log("FolAng.start() targ=%d vel=%d", _target, _origSpeed);
  _correcting = false;
}


void FollowAngle::loop() {
  _sensors->loop(COMPASS);
  
  double drift = calcDrift(_sensors->getAngle(), _target);
  
  if (abs(drift) > MAX_DRIFT && !_correcting) {
    _driver->correct(drift > 0? RIGHT : LEFT);
    _correcting = true;
  }
  else if (_correcting && abs(drift) < 5) {
    _log("FolAng.stop corr: drift=%d", drift);
    _driver->go(_origSpeed);
    _correcting = false;
  }
}

FollowWall::FollowWall(Driver &driver, Sensors &sensors, side_T sideToFollow) {
  _driver = &driver;
  _sensors = &sensors;
  _sideToFollow = sideToFollow;
}

void FollowWall::start() {
  side_T sides[] = { LEFT, RIGHT };
  
  if (_sideToFollow == NONE) {
    _sensors->loop(ULTRASONIC);
    _target = MAX_CM;

    for (int i = 0; i < 2; i++) {
      side_T side = sides[i];
      int d = _sensors->getDistance()[SIDE2DISTANCE(side)];
      if (d < _target) {
        _target = d;
        _sideToFollow = side;
      }
    }
  }
}

void FollowWall::loop() {
  /* TODO */
}

int FollowAngle::calcDrift(double curAngle, int targetAngle) {
  int drift = int(curAngle) - targetAngle;

  if (drift > 180) {
    drift = calcDrift(curAngle, targetAngle + 360);
  } else if (drift < -180) {
    drift = calcDrift(curAngle + 360, targetAngle);
  }
  return drift;
}

TargetAngle::TargetAngle(Sensors &sensors, int shift) {
  _sensors = &sensors;
  _shift = shift;
}

void TargetAngle::start() {
  _sensors->loop(COMPASS);
  _target = ( (int)_sensors->getAngle() + _shift) % 360;
  if (_target < 0) {
    _target += 360;
  }
  _log("TarAng.start() tar=%d cur=%d", _target, (int) _sensors->getAngle());
}

bool TargetAngle::eval() {
  _sensors->loop(COMPASS);
  return(abs(_sensors->getAngle() - _target) < 5);
}
