#include <Arduino.h>
#include <Servo.h>
#include "config.h"

int pin = SERVO;
const int DOWN = SERVO_DOWN;
const int UP = SERVO_UP;

Servo myservo;

void setup_servo()
{
  myservo.attach(pin);
  myservo.write(UP);
}

void shutdown_servo()
{
  myservo.write(UP);
}

void move_servo(int pos)
{
  myservo.write(pos);
}
