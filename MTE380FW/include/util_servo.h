#include <Arduino.h>
#include <Servo.h>

const int SERVO = PB6; // D10, PWM

const int DOWN = 80;
const int UP = 0;

Servo gate;

void setup_servo()
{
  gate.attach(SERVO);
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
