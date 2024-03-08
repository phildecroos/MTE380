#include <Arduino.h>
#include <Servo.h>
#include "config.h"

int pin = SERVO;
int down = SERVO_DOWN;

Servo myservo;

void setup_servo()
{
  myservo.attach(pin);
  myservo.write(down);
}

void shutdown_servo()
{
  myservo.write(down);
}

void move_servo(int pos)
{
  myservo.write(pos);
}
