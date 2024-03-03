#include <Arduino.h>
#include <Servo.h>
#include "config.h"

Servo myservo;

void setup_servo()
{
  int pin = SERVO;
  myservo.attach(pin);
}

// TODO - add logic here to map inputs to up/down pos of gate
void move_servo(int pos)
{
  myservo.write(pos);
}