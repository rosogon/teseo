#include "driver.h"
#include <log.h>

template <typename ... Args> static void _log(const char *format, Args... args) {
  tlog("DRI - ", format, args...);
}

Driver::Driver(MeDCMotor *left, MeDCMotor *right, Sensors *sensors) {

  this->sensors = sensors;
  leftM = left;
  rightM = right;
}

bool Driver::isIdle() {
  return !_action;
}

void Driver::run(Action& action, Loop& loop, Condition& until) {
  _log("run(%s, %s, %s)", action.name(), loop.name(), until.name());
  _action = &action;
  _loop = &loop;
  _until = &until;
}

void Driver::loop() {
  if (isIdle()) {
    _log("loop() : isIdle");
    return;
  }

  if (!_action->isStarted()) {
    _log("loop() : starting action");
    _action->start();
    _loop->start();
    _until->start();
  }
  
  if (_until->eval()) {
    _log("loop() : condition met");
    _action->stop();
    _action = nullptr;
    _until = nullptr;
    _loop = nullptr;
  } else {
      _loop->loop();
  }
}


void Driver::stop() {
  _stop();
}

void Driver::go(int speed) {
  _log("go(%d)", speed);
  move2(speed, FORWARD);
}

void Driver::back(int speed) {
  _log("back(%d)", speed);
  move2(speed, BACKWARDS);
}

void Driver::turn(side_T side, int speed) {
  _log("turn(%s, %d)", dirName(side), speed);
  move2(speed, TURN, side);
}

void Driver::spin(side_T side, int speed) {
  _log("spin(%s, %d)", dirName(side), speed);
  move2(speed, SPIN, side);
}

void Driver::correct(side_T side) {
  /*
   * Hardwired amounts for the moment
   */
  int left = leftSpeed, right = rightSpeed;

  correctionSpeeds(&left, &right, side, 50);
  _log("correct(%s) - new speeds: L=%d R=%d", dirName(side), left, right);
  setSpeed(left, right);  
}

void Driver::correctionSpeeds(int *left, int *right, side_T side, byte amount) {
  if (side > 0) {
    *right += amount;
    *left -= amount;
  } else {
    *left += amount;
    *right -= amount;
  }
}

void Driver::_stop() {
  move2(0, FORWARD);
}

/*
 * Return the value<side> that matches the side parameter
 */
inline int _ifside(side_T side, int valueLeft, int valueRight, int valueNone) {
  if (side == LEFT) {
    return valueLeft;
    
  } else if (side == RIGHT) {
    return valueRight;
    
  } else {
    return valueNone;
  }
}

/*        
 *        FORW     |  BACK       |  TURN   |  SPIN
 * LEFT | s-d, s   |  -s+d, -s   |  d, s   |  -s, s
 * RIGHT| s,   s-d |  -s  , -s+d |  s, d   |  s, -s
 * NONE | s,   s   |  -s  , -s   |  0, 0   |  0 , 0
 */
void Driver::move2(int speed, action_T action, side_T side, byte delta) {
  int leftSpeed = 0;
  int rightSpeed = 0;

  switch(action) {
    case FORWARD:
      leftSpeed  = _ifside(side, speed - delta, speed, speed);
      rightSpeed = _ifside(side, speed, speed - delta, speed);
      break;
    case BACKWARDS:
      leftSpeed  = _ifside(side, -(speed - delta), -speed, -speed);
      rightSpeed = _ifside(side, -speed, -(speed - delta), -speed);
      break;
    case TURN:
      leftSpeed = _ifside(side, delta, speed, 0);
      rightSpeed = _ifside(side, speed, delta, 0);
      break;
    case SPIN:
      leftSpeed = _ifside(side, -speed, speed, 0);
      rightSpeed = _ifside(side, speed, -speed, 0);
      break;
    default:
      break;
  }
  setSpeed(leftSpeed, rightSpeed);
}

//void Driver::move(side_T direction, int speed, bool turnOrSpin) {
//  
//  _log("move(%s, %d, turnOrSpin=%d)", dirName(direction), speed, turnOrSpin);
//  int leftSpeed = 0;
//  int rightSpeed = 0;
//  switch (direction) {
//    case AHEAD:
//      leftSpeed = speed;
//      rightSpeed = speed;
//      break;
//    case BACK:
//      leftSpeed = -speed;
//      rightSpeed = -speed;
//      break;
//    case LEFT:
//      leftSpeed = -speed * turnOrSpin;
//      rightSpeed = speed;
//      break;
//    case RIGHT:
//      leftSpeed = speed;
//      rightSpeed = -speed * turnOrSpin;
//      break;
//    default:
//      /* stop */
//      break;
//  }
//  setSpeed(leftSpeed, rightSpeed);
//}

void Driver::setSpeed(int left, int right) {
  _log("setSpeed(L=%d, R=%d)", left, right);
  leftM->run(-left);
  rightM->run(right);
  this->leftSpeed = left;
  this->rightSpeed = right;
}
