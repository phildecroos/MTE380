#include <Arduino.h>
#include <Servo.h>
#include "pinout.h"

int pin = SERVO;
const int DOWN = 80;
const int UP = 0;

Servo gate;

void setup_servo()
{
  gate.attach(pin);
  gate.write(UP);
}

void shutdown_servo()
{
  gate.write(UP);
  delay(500);
  gate.detach();
}

void move_servo(int pos)
{
  gate.write(pos);
}
