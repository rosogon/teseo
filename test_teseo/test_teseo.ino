#include <SoftwareSerial.h>

#include <Arduino.h>                                                            
#include <SoftwareSerial.h>                                                     
#include <Wire.h>
#include <MeMCore.h>
#include <LiquidCrystal_I2C.h>
#include <ultrasonic.h>

#define I2C_ADDR    0x27
#define MAX_CM 200


UltrasonicSensor usR(PORT_3, MAX_CM);
UltrasonicSensor usL(PORT_2, MAX_CM);
UltrasonicSensor usF(PORT_1, MAX_CM);
MeCompass compass(PORT_4);
LiquidCrystal_I2C lcd(I2C_ADDR,16, 2);

UltrasonicSensor* uss[] = { &usF, &usR, &usL };
UltrasonicSet serialSet(uss, 3, false);
UltrasonicSet parallelSet(uss, 3, true);

void setup() {
    Serial.begin(115200);
    compass.begin();
    usF.setPins(usF.pin2(), usF.pin2());
    lcd.init();
    lcd.backlight();
    lcd.home();
    lcd.setCursor(0, 0);
    lcd.print("F=  L=  R=  A=  ");
}

void checkSensorValues() {
  char str[17];

  int dF = usF.distanceCm();
  int dL = usL.distanceCm();
  int dR = usR.distanceCm();
  double angle = compass.getAngle();
  Serial.println("dF=" + String(dF) + "\tdL=" + String(dL) + "\tdR=" + String(dR) + "\tA=" + String(angle));
  
  snprintf(str, 16, "%3d %3d %3d %3d", int(round(dF)), int(round(dL)), int(round(dR)), int(round(angle)));
  Serial.println(str);
  lcd.setCursor(0, 1);
  lcd.print(str);
  delay(1000);
}

long checkTimesPerSecond(bool checkUs, bool checkCompass, int parallelType) {

  long nextSecond = millis() + 1000;
  long times = 0;
  
  while (1) {
    
    if (checkUs) {
      if (parallelType) {
        parallelSet.parallelType = parallelType;
        int* d2 = parallelSet.distanceCm();
      }
      else {
        int *d1 = serialSet.distanceCm();

      }
      delay(23);
    }
    if (checkCompass) {
      double angle = compass.getAngle();  
    }
    
    times++;
    long now = millis();
    if (now > nextSecond) {
      return times;
    }
  }
}

void checkTimesPerSecondLoop() {

  for (int type = 0; type < 3; type++) {
    long times[4];
    for (int i = 0; i < 4; i++) {
      times[i] = checkTimesPerSecond( i & 1, i & 2, type);
    }
  
    String s = "(" + String(type) + ") ";
    for (int i = 0; i < 4; i++) {
      s = s + "times[" + String(i) + "]=" + String(times[i]) + "\t";
    }
    Serial.println(s);
  }
}


void checkSyncVsAsync() {
  parallelSet.parallelType = 1;
  int* d2 = parallelSet.distanceCm();

  delay(100);
  int *d1 = serialSet.distanceCm();

  char str[100] = "";
  snprintf(str, 100, "F(s,a)=(%d,%d)\tR(s,a)=(%d,%d)\tL(s,a)=(%d,%d)", d1[0], d2[0], d1[1], d2[1], d1[2], d2[2]);
  Serial.println(str);
  delay(1000);
}

void loop() {
  checkSensorValues();
  //checkTimesPerSecondLoop();
  //checkSyncVsAsync();
}
