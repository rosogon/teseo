#include "actions.h"
#include <log.h>

static void _log(const char *format, ...) {
    LOG_VA("ACT - ", format);
}

/*
 * Go 
 */

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
  _started = false;
}

bool Go::isStarted() {
  return _started;
}

/*
 * Move
 */

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
  _started = false;
}

bool Move::isStarted() {
  return _started;
}

/*
 * MeasureWall
 */

MeasureWall::MeasureWall(Sensors &sensors) {
  _sensors = &sensors;
}

void MeasureWall::start() {
  _sensors->loop(ULTRASONIC);
}

void MeasureWall::loop() {
  _sensors->loop(ULTRASONIC);
}

/*
 * Elapsed
 */

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

/*
 * NoWall
 */

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

/*
 * FollowAngle
 */

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

int FollowAngle::calcDrift(double curAngle, int targetAngle) {
  int drift = int(curAngle) - targetAngle;

  if (drift > 180) {
    drift = calcDrift(curAngle, targetAngle + 360);
  } else if (drift < -180) {
    drift = calcDrift(curAngle + 360, targetAngle);
  }
  return drift;
}

/*
 * FollowWall
 */

FollowWall::FollowWall(Driver &driver, Sensors &sensors, side_T sideToFollow, 
    double kp, double ki, double kd) {
  _driver = &driver;
  _sensors = &sensors;
  _sideToFollow = sideToFollow;
  this->kp = kp;
  this->ki = ki;
  this->kd = kd;
}

void FollowWall::start() {
    
  if (_sideToFollow == NONE) {
    _sensors->loop(ULTRASONIC);
    _target = MAX_CM;

    side_T sides[] = { LEFT, RIGHT };
    for (int i = 0; i < 2; i++) {
      side_T side = sides[i];
      int d = _sensors->getDistance()[SIDE2DISTANCE(side)];
      if (d < _target) {
        _target = d;
        _sideToFollow = side;
      }
    }
  } else {
    _sensors->loop(ULTRASONIC);
    int pos = SIDE2DISTANCE(_sideToFollow);
    _target = _sensors->getDistance()[pos];
  }
  d0 = _target;
  t0 = millis();
  e0 = 0;
  sE = 0;
}

void FollowWall::loop() {
  _sensors->loop(ULTRASONIC);
  int d = _sensors->getDistance()[SIDE2DISTANCE(_sideToFollow)];
  unsigned long t = millis();
  long int dT = t - t0;

  /*
   * If e > 0, we are closer to the wall
   */

  int e = _target - d;
  double dE = (e - e0) * 1000.0 / (double)dT;
  sE += e * dT / 1000.0;

  double kp_e = kp * e;
  double ki_sE = ki * sE;
  double kd_dE = kd * dE;
  _log("t0=%lu t=%lu", t0, t);
  //_log("e=%d kp=%lf ki=%lf kd=%lf", e, kp, ki, kd);
  //_log("kp*e=%lf ki*sE=%lf dE=%lf kd*dE=%lf dT=%ld", kp_e, ki_se, dE, kd_de, dT);
  std::cerr 
    << "kp*e=" << kp << "*" << e << "=" << kp_e 
    << " ki*sE=" << ki << "*" << sE << "=" << ki_sE 
    << " kd*de=" << kd << "*" << dE << "=" << kd_dE << std::endl;
  std::cerr << "t0=" << t0 << " t=" << t << " dT=" << dT << std::endl;
  int o = kp_e + ki_sE + kd_dE;
  //int o = (kp * e) + (ki * sE) + (kd * dE);
  e0 = e;
  t0 = t;
  d0 = d;
  o0 = o;
  int speed = 255 - o;  // this should be orig_speed
  byte delta = 0;
  side_T sideToTurn;

  if (o > 0) {
    delta = o;
    sideToTurn = (side_T) -_sideToFollow;
  } else {
    delta = -o;
    sideToTurn = _sideToFollow;
  }
  _driver->move2(speed - delta, FORWARD, sideToTurn, delta);
}

/*
 * TargetAngle
 */

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
