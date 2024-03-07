#include <Arduino.h>
#include "config.h"

int ena_l = MOTOR_ENA_L;
int for_l = MOTOR_F_L;
int back_l = MOTOR_B_L;
int ena_r = MOTOR_ENA_R;
int for_r = MOTOR_F_R;
int back_r = MOTOR_B_R;
float steer_max = MOTOR_STEER;

void setup_motors()
{
  pinMode(ena_l, OUTPUT);
  pinMode(for_l, OUTPUT);
  pinMode(back_l, OUTPUT);
  pinMode(ena_r, OUTPUT);
  pinMode(for_r, OUTPUT);
  pinMode(back_r, OUTPUT);

  analogWrite(ena_l, 0);
  analogWrite(ena_r, 0);
  digitalWrite(for_l, LOW);
  digitalWrite(back_l, LOW);
  digitalWrite(for_r, LOW);
  digitalWrite(back_r, LOW);
}

void shutdown_motors()
{
  analogWrite(ena_l, 0);
  analogWrite(ena_r, 0);
  digitalWrite(for_l, LOW);
  digitalWrite(back_l, LOW);
  digitalWrite(for_r, LOW);
  digitalWrite(back_r, LOW);
}

/*
gear
- see enum in main for values
- does nothing if inputted gear value is not one of these 3
- forward drives both motors forward, steering by slowing one motor (min 0)
- reverse drives both motors backward, steering by slowing one motor (min 0)
- inplace drives one motor forward and one motor backward both at same speed proportional to steer

steer
- int, ranging -MOTOR_STEER to MOTOR_STEER (in config)
- 0 is straight, positive is right, negative is left
- larger magnitude is a sharper steer

speed
- int, 0 to 255
- controls pwm duty cycle (0% to 100%)
- driving in forward/reverse uses MOTOR_SPEED (in config)
- regardless of gear & steer, inputting speed==0 will stop the robot
*/

void drive_motors(int gear, int steer, int speed)
{
  if (gear == 2) // inplace
  {
    if (steer == 0) // sit still
    {
      analogWrite(ena_l, 0);
      analogWrite(ena_r, 0);
    }
    else if (steer > 0) // turn right (clockwise from top) in place
    {
      digitalWrite(for_l, HIGH);
      digitalWrite(back_l, LOW);
      digitalWrite(for_r, LOW);
      digitalWrite(back_r, HIGH);
      analogWrite(ena_l, (speed * (steer / steer_max)));
      analogWrite(ena_r, (speed * (steer / steer_max)));
    }
    else // turn left (counter-clockwise from top) in place
    {
      digitalWrite(for_l, LOW);
      digitalWrite(back_l, HIGH);
      digitalWrite(for_r, HIGH);
      digitalWrite(back_r, LOW);
      analogWrite(ena_l, (speed * (steer / steer_max)));
      analogWrite(ena_r, (speed * (steer / steer_max)));

      return;
    }
  }
  else if (gear == 0) // forward
  {
    digitalWrite(for_l, HIGH);
    digitalWrite(back_l, LOW);
    digitalWrite(for_r, HIGH);
    digitalWrite(back_r, LOW);
    float l_speed;
    float r_speed;

    if (steer == 0) // drive straight
    {
      l_speed = speed;
      r_speed = speed;
    }
    else if (steer > 0) // steer right
    {
      l_speed = speed;
      r_speed = (speed * ((steer_max - steer) / steer_max));
    }
    else // steer left
    {
      l_speed = (speed * ((steer_max - abs(steer)) / steer_max));
      r_speed = speed;
    }
    Serial.println("L speed: ");
    Serial.println(l_speed);
    Serial.println("R speed: ");
    Serial.println(r_speed);
    analogWrite(ena_l, l_speed);
    analogWrite(ena_r, r_speed);
  }
  else if (gear == 1) // reverse
  {
    digitalWrite(for_l, LOW);
    digitalWrite(back_l, HIGH);
    digitalWrite(for_r, LOW);
    digitalWrite(back_r, HIGH);

    if (steer == 0) // drive straight
    {
      analogWrite(ena_l, speed);
      analogWrite(ena_r, speed);
    }
    else if (steer > 0) // steer right
    {
      analogWrite(ena_l, speed);
      analogWrite(ena_r, (speed * ((steer_max - steer) / steer_max)));
    }
    else // steer left
    {
      analogWrite(ena_l, (speed * ((steer_max - steer) / steer_max)));
      analogWrite(ena_r, speed);
    }
  }
  else
  {
    Serial.println("WARNING: Unknown gear value inputted: ");
    Serial.println(gear);
  }
}
