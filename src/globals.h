#include <Arduino.h>
#ifndef globals_h
#define globals_h

const byte relisePumpGluecksfeeder = 20;
unsigned long previousMillis=millis();
// #define trigger1 5
// #define echo1 18

int trigger1 =5;
int echo1 = 18;

long  distance1 = 0;

enum states{
  GETDATA,
  PUMPING,
  TIMEFORSLEEP,
  WAIT,
} state;


#endif
