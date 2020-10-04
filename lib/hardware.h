#ifndef hardware_h
#define hardware_h

#include <Arduino.h>

#ifdef __AVR__

#include <MePort.h>
#include <MeCompass.h>
#include <MeDCMotor.h>

#else

#include <arduinomock.h>

#endif

#endif