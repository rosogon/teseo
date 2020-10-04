#ifndef driver_h
#define driver_h

#include <hardware.h>
#include "sensors.h"

typedef enum : char {
  LEFT = -1,
  BACK = 0,
  NONE = 0,
  RIGHT = 1,
  AHEAD = 2
} side_T;

typedef enum : char {
  FORWARD = 2,
  BACKWARDS = 0,
  TURN = 1,
  SPIN = 3
} action_T;

#define SIDE2DISTANCE(s) ( ((s) & 3) -1 )

static const char* dirNames[] = {
  "LE",
  "NO",
  "RI",
  "ERR"
};

static const char* actionNames[] = {
  "FW",
  "BW",
  "TU",
  "SP",
  "ERR"
};

inline const char* dirName(side_T side) {
  if (side < LEFT || side > AHEAD) {
    return dirNames[RIGHT + 2];
  }
  return dirNames[side + 1];
}

inline const char* actionName(action_T action) {
  if (action < FORWARD || action > SPIN) {
    return actionNames[SPIN + 1];
  }
  return actionNames[action];
}

class Action {
public:
  virtual const char *name() {
    return "Action";
  }
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual bool isStarted() = 0;
};

class Loop {

public:
  virtual const char *name() {
    return "Loop";
  }
  virtual void start() {}
  virtual void loop() = 0;
};

class NoOp : public Loop {

public:
  virtual const char *name() {
    return "NoOp";
  }
  void loop() {}
};

class Condition {

public:
  virtual const char *name() {
    return "Cond";
  }
  virtual void start() {}
  virtual bool eval() = 0;
};

class Or {
  Condition *_t1;
  Condition *_t2;
public:
  Or(Condition &t1, Condition &t2) {
    _t1 = &t1;
    _t2 = &t2;
  }

  void start() {
    _t1->start();
    _t2->start();
  }
  
  bool eval() {
    return _t1->eval() || _t2->eval();
  }
};

class  Driver {

public:
  Driver(MeDCMotor *left, MeDCMotor *right, Sensors *sensors);
  inline bool isWarding();
  inline bool isMoving();
  inline bool isCorrecting();
  int left() { return leftSpeed; }
  int right() { return rightSpeed; }
  
  bool isIdle();
  void loop();
  void move2(int speed, action_T action, side_T side = NONE, byte delta = 0);

  void stop();
  void go(int speed);
  void back(int speed);
  void spin(side_T side, int speed);
  void turn(side_T side, int speed);
  void correct(side_T direction);

  void run(Action&, Loop&, Condition&);

private:
  MeDCMotor *leftM;
  MeDCMotor *rightM;
  Sensors *sensors;
  unsigned long until;

  Action* _action;
  Loop* _loop;
  Condition* _until;
  
  int leftSpeed = 0, 
      rightSpeed = 0;

  void _stop();
  int calcDrift(double curAngle, int targetAngle);
  void correctionSpeeds(int *left, int *right, side_T side, byte amount);

  /*
   * Set motor speeds according to desired direction and action.
   * 
   * Turn means one motor is active and the other is stopped.
   * Spin means both motors are active with same speed but opposite directions.
   * 
   * direction: one of AHEAD, BACK, LEFT, RIGHT
   *   AHEAD and BACK means both motors have the same speed.
   *   LEFT and RIGHT depends on turnOrSpin param.
   * speed: motor speed (actual speed on each motor depends on direction and turnOrSpin)
   * turnOrSpin: if true, turns, else spins. 
   * 
   */
//  void move(side_T direction, int speed, bool turnOrSpin = false);

  void setSpeed(int left, int right);

  void push(Action *action);
  Action* pop();
};


#endif
