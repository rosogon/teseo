
#include <Arduino.h>
#include <MeMCore.h>
#include <Wire.h>
#include <ultrasonic.h>
#include "sensors.h"
#include <times.h>
#include "driver.h"
#include "actions.h"
#include <log.h>

#define MAX_CM 200

const char *mainstr = "MAIN - ";

static void _log(const char *format, ...) {
    LOG_VA("MAIN - ", format);
}

MeCompass compass(PORT_4);
UltrasonicSensor usR(PORT_3, MAX_CM);
UltrasonicSensor usL(PORT_2, MAX_CM);
UltrasonicSensor usF(PORT_1, MAX_CM);

UltrasonicSensor* uss[] = { &usR, &usF, &usL };
UltrasonicSet ultrasonicSet(uss, 3, true);

Sensors sensors(&compass, 50, &ultrasonicSet, 100);

MeDCMotor motorL(M1);
MeDCMotor motorR(M2);
Driver driver(&motorL, &motorR, &sensors);

//Go action(driver, 100);
Move action(driver, 100, BACKWARDS);
NoOp correction;
//MeasureWall correction1(sensors);
//FollowAngle correction2(driver, sensors);
Elapsed condition(1000);
//NoWall condition1(sensors);
//TargetAngle condition(sensors, LEFT * 90);


extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

uint16_t getFreeSram() {
  uint8_t newVariable;
  // heap is empty, use bss as start memory address
  if ((uint16_t)__brkval == 0)
    return (((uint16_t)&newVariable) - ((uint16_t)&__bss_end));
  // use heap end as the start of the memory address
  else
    return (((uint16_t)&newVariable) - ((uint16_t)__brkval));
};


inline bool buttonPressed() {
  return analogRead(A7) <= 10;
}

void setup() {
  Serial.begin(115200);
  tlog("MAIN - ", "setup()");
  usF.setPins(usF.pin2(), usF.pin2());
  compass.begin();
  driver.stop();
}

void print() {
  int* d = sensors.getDistance();
  int angle = (int) sensors.getAngle();

  tlog(mainstr, "dF=%d\tdR=%d\tdL=%d\tA=%d", d[1], d[0], d[2], angle);
}

void loop() {
  const char *freeram = "FreeRAM=%u";
  tlog(mainstr, "Start");
  Times tPrint;
  tPrint.start(1000);
  bool _exit = false;
  do {
    sensors.loop(ULTRASONIC | COMPASS);
    if (tPrint.isLap() != -1) {
      print();
      tlog(mainstr, freeram, getFreeSram());
    }
    for (int i = 0; i < 10; i++) {
      if (buttonPressed()) {
        _exit = true;
      }
      delay(50);
    }
  } while (!_exit);
  delay(1000);

  driver.run(action, correction, condition);
  //sensors.loop(ULTRASONIC |  COMPASS);
  while (1) {
    driver.loop();
    if (tPrint.isLap() != -1) {
      print();
    }
    if (driver.isIdle()) {
      tlog(mainstr, "Driver idle");
      break;
    }
      
    if (buttonPressed()) {
      tlog(mainstr, "Button pressed");
      break;
    }
  }
  tlog(mainstr, freeram, getFreeSram());
  delay(1000);
  print();
//  driver.go(2000);
//  while (1) {
//    sensors.loop(ULTRASONIC | COMPASS);
//    driver.loop();
//    if (driver.isIdle()) {
//      break;
//    }
//  }
//  sensors.loop(ULTRASONIC | COMPASS);
//  Times tPrint;
//  tPrint.start(500);
//  while (1) {
//    delay(0.1);
//    sensors.loop(ULTRASONIC | COMPASS);
//    if (tPrint.isLap() != -1) {
//      print();
//    }
//  }
    
  driver.stop();
  do {
    delay(10);
  } while (!buttonPressed());
  tlog("MAIN - ", "End");
}
