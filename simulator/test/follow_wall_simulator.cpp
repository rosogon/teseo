#include <iostream>
#include <fstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <math.h>

#include <Print.h>
#include <hardware.h>
#include <log.h>
#include "../../teseo/driver.h"
#include "../../teseo/actions.h"
#include "../../teseo/test/mocks.h"

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)

static void _log(const char *format, ...) {
    LOG_VA("TEST - ", format);
}

std::string exec(const char* cmd);

StderrPrint mockPrint;

GodmodeState* state = GODMODE();
MeDCMotor left(M1);
MeDCMotor right(M2);
UltrasonicSensorMock usL;
UltrasonicSensorMock usR;
MeCompass compass(PORT_4);
UltrasonicSensor* uss[] = { &usR, nullptr, &usL };
UltrasonicSet ultrasonicSet(uss, 3, false);

Sensors sensors(&compass, 50, &ultrasonicSet, 500);
Driver d(&left, &right, &sensors);
FollowWall *loop;

std::fstream dataOut;
const char *dataPath = "data.csv";

long delay_ms = 500;
int alfa0 = 45;
int speed0 = 255;
double r_x = 0;
double r_y = 10;
double l_x;
double l_y;
double alfa;

int n = 10;
double kp = 1;
double ki = 0.1;
double kd = 10;

void initialize() {
  setConsole(mockPrint);

  dataOut.open(dataPath, std::fstream::out | std::fstream::trunc);

  d.move2(speed0, FORWARD);
  state->reset();
  state->resetClock();
  state->resetPins();

  delay(delay_ms);
  usR.nextValue(r_y);
  (d, sensors, RIGHT);
  loop = new FollowWall(d, sensors, RIGHT, kp, ki, kd);
  loop->start();
  _log("Error=%d Output=%d", loop->getError(), loop->getOutput());
}


void parse_output(std::string output, double *plf_x, double *plf_y, double *prf_x, double *prf_y, double *alfa) {
  //std::cout << "Received: " << output << "\n";
  int n = sscanf(output.c_str(), "plf=%lf,%lf;prf=%lf,%lf;alfa=%lf", plf_x, plf_y, prf_x, prf_y, alfa);
  _log("Read: %d plf=%lf,%lf; prf=%lf,%lf; alfa=%lf", n, *plf_x, *plf_y, *prf_x, *prf_y, *alfa);
}

void printToFile() {
  dataOut << l_x << " " << l_y << std::endl;
  dataOut << r_x << " " << r_y << std::endl;
}

void doFirstStep() {

  char cmd[1024];
  sprintf(cmd, "python3 simulator.py ? %d %.4lf,%.4lf %d", alfa0, r_x, r_y, d.right());
  _log("%s", cmd);
  std::string out = exec(cmd);

  parse_output(out, &l_x, &l_y, &r_x, &r_y, &alfa);

  delay(delay_ms);
  usR.nextValue(r_y);
  loop->loop();
  _log("Error=%d Output=%d (L,R)=(%d, %d)\n", loop->getError(), loop->getOutput(), d.left(), d.right());
  printToFile();
}


void doStep() {
  char cmd[1024];
  sprintf(cmd, "python3 simulator.py  %.4lf,%.4lf %d %.4lf,%.4lf %d", 
      l_x, l_y, d.left(), r_x, r_y, d.right());
  _log("%s", cmd);
  std::string out = exec(cmd);

  parse_output(out, &l_x, &l_y, &r_x, &r_y, &alfa);

  delay(delay_ms);
  usR.nextValue(r_y);
  loop->loop();
  _log("Error=%d Output=%d (L,R)=(%d, %d)\n", loop->getError(), loop->getOutput(), d.left(), d.right());
  printToFile();
}


std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
      throw std::runtime_error("popen() failed!");
    }
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

void usage(char *program_name) {
  std::cout << "Usage: " << program_name << " [-a angle] [-p kP] [-i kI] [-d kD] [-n n_iterations]";
  std::cout << std::endl;
}

int main(int argc, char *argv[]) {
  printf("Simulator :)\n");
 
  int opt;
  while ( (opt = getopt(argc, argv, "ha:p:i:d:n:")) != -1) {
    switch(opt) {
      case 'h':
        usage(argv[0]);
        exit(1);
      case 'a':
        alfa0 = atoi(optarg);
        break;
      case 'p':
        kp = atof(optarg);
        break;
      case 'i':
        ki = atof(optarg);
        break;
      case 'd':
        kd = atof(optarg);
        break;
      case 'n':
        n = atoi(optarg);
        break;
    }
  }

  initialize();
  doFirstStep();
  for (int i = 0; i < n; i++) {
    doStep();
  }
  dataOut.close();
}