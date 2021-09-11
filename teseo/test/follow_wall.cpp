#include <ArduinoUnitTests.h>
#include <unistd.h>
#include <Print.h>
#include <hardware.h>
#include <log.h>
#include "../driver.h"
#include "../actions.h"
#include "mocks.h"

static void _log(const char *format, ...) {
    LOG_VA("TEST - ", format);
}

StderrPrint mockPrint;

unittest(follow_wall) {
  setConsole(mockPrint);

  GodmodeState* state = GODMODE();
  state->reset();
  state->resetClock();
  state->resetPins();

  MeDCMotor left(M1);
  MeDCMotor right(M2);
  UltrasonicSensorMock usL;
  MeCompass compass(PORT_4);

  UltrasonicSensor* uss[] = { nullptr, nullptr, &usL };
  UltrasonicSet ultrasonicSet(uss, 3, false);

  Sensors sensors(&compass, 50, &ultrasonicSet, 100);
  Driver d(&left, &right, &sensors);

  FollowWall loop(d, sensors, LEFT);

  delay(100);
  usL.nextValue(10);
  loop.start();
  _log("Error=%d Output=%d", loop.getError(), loop.getOutput());

  delay(100);
  usL.nextValue(15);
  loop.loop();
  _log("Last=%d Error=%d Output=%d", loop.getLastDistance(), loop.getError(), loop.getOutput());
  _log("L=%d R=%d", d.left(), d.right());

  assertEqual(-5, loop.getError());

  delay(100);
  usL.nextValue(5);
  loop.loop();
  _log("Last=%d Error=%d Output=%d", loop.getLastDistance(), loop.getError(), loop.getOutput());
  _log("L=%d R=%d", d.left(), d.right());

  assertEqual(5, loop.getError());
}

unittest_main()
