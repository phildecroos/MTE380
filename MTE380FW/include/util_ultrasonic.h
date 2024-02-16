#include <Arduino.h>
#include "config.h"

void setup_ultrasonic() {
  int trigger = US_TRIGGER;
  int echo = US_ECHO;
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
}

int read_ultrasonic() {
  int trigger = US_TRIGGER;
  int echo = US_ECHO;
  long duration;
  int distance;

  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
  return distance;
}
