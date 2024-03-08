#include <Arduino.h>
#include "config.h"

int trigger = US_TRIGGER;
int echo = US_ECHO;

void setup_ultrasonic()
{
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
}

void shutdown_ultrasonic()
{
  digitalWrite(trigger, LOW);
}

int read_ultrasonic()
{
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
