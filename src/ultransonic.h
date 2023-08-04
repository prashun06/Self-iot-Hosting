#include <Arduino.h>
#include "globals.h"

long disitanceMeasure(int trigger, int echo){
    long duration = 0;
    long dist = 0;
    digitalWrite(trigger, LOW);
    delay(5);
    digitalWrite(trigger, HIGH);
    delay(10);
    digitalWrite(trigger, LOW);
    duration = pulseIn(echo, HIGH);

    dist = (duration/2)/29.1;
    return dist;
}

void getDistance(){
  distance1 = disitanceMeasure(trigger1, echo1);
  //distance2 = disitanceMeasure(trigger2, echo2);
}
