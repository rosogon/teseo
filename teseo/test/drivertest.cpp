#include <ArduinoUnitTests.h>
#include "../driver.h"

unittest(driver_move_forward)
{
  GodmodeState* state = GODMODE();
  state->reset();
  state->resetClock();
  state->resetPins();

  MeDCMotor left(M1);
  MeDCMotor right(M2);
  UltrasonicSensor usF(PORT_1, MAX_CM);
  MeCompass compass(PORT_4);

  UltrasonicSensor* uss[] = { &usF };
  UltrasonicSet ultrasonicSet(uss, 1, false);

  Sensors sensors(&compass, 50, &ultrasonicSet, 100);
  Driver d(&left, &right, &sensors);

  /* Left motor values are always negated */
  d.move2(255, FORWARD);

  assertEqual(-255, left.speed());
  assertEqual(255, right.speed());
  assertEqual(255, d.left());
  assertEqual(255, d.right());

  d.move2(255, FORWARD, LEFT, 55);
  assertEqual(255, d.right());
  assertEqual(200, d.left());

  d.move2(255, FORWARD, RIGHT, 55);
  assertEqual(255, d.left());
  assertEqual(200, d.right());

  d.move2(255, FORWARD, RIGHT, 0);
  assertEqual(255, d.left());
  assertEqual(255, d.right());

  d.move2(255, FORWARD, BACK, 55);
  assertEqual(255, d.left());
  assertEqual(255, d.right());

  d.move2(255, BACKWARDS);
  assertEqual(-255, d.left());
  assertEqual(-255, d.right());

  d.move2(255, BACKWARDS, LEFT, 55);
  assertEqual(-255, d.right());
  assertEqual(-200, d.left());

  d.move2(255, BACKWARDS, RIGHT, 55);
  assertEqual(-255, d.left());
  assertEqual(-200, d.right());

  d.move2(255, BACKWARDS, NONE, 55);
  assertEqual(-255, d.left());
  assertEqual(-255, d.right());

  d.move2(200, TURN, LEFT);
  assertEqual(200, d.right());
  assertEqual(0, d.left());

  d.move2(200, TURN, LEFT, 100);
  assertEqual(200, d.right());
  assertEqual(100, d.left());

  d.move2(200, TURN, RIGHT);
  assertEqual(0, d.right());
  assertEqual(200, d.left());

  d.move2(200, TURN, RIGHT, 100);
  assertEqual(100, d.right());
  assertEqual(200, d.left());

  d.move2(200, SPIN, LEFT);
  assertEqual(200, d.right());
  assertEqual(-200, d.left());

  d.move2(200, SPIN, RIGHT);
  assertEqual(200, d.left());
  assertEqual(-200, d.right());
}

unittest_main()