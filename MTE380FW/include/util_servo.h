#include <Arduino.h>
#include <Servo.h>
#include "config.h"

int pin = SERVO;
int down = SERVO_DOWN;
int up = SERVO_UP;

Servo myservo;

void setup_servo()
{
  myservo.attach(pin);
  myservo.write(up);
}

void shutdown_servo()
{
  myservo.write(up);
}

void move_servo(int pos)
{
  myservo.write(pos);
}
