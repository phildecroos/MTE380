#include <Arduino.h>
#include "config.h"

int mode = MOTOR_MODE;
int ena_l = MOTOR_ENA_L;
int dir_l = MOTOR_DIR_L;
int ena_r = MOTOR_ENA_R;
int dir_r = MOTOR_DIR_R;
float steer_max = MOTOR_STEER;
const float REV_MAX = -30.0;

float l_speed;
float r_speed;

// all speed buffs must be 0 to 1
const float INP_L_BUFF = 1.0;
const float INP_R_BUFF = 1.0;
const float FWD_L_BUFF = 0.9;
const float FWD_R_BUFF = 1.0;
const float REV_L_BUFF = 1.0;
const float REV_R_BUFF = 1.0;

void setup_motors()
{
  pinMode(mode, OUTPUT);
  pinMode(ena_l, OUTPUT);
  pinMode(dir_l, OUTPUT);
  pinMode(ena_r, OUTPUT);
  pinMode(dir_r, OUTPUT);

  digitalWrite(mode, HIGH);
  analogWrite(ena_l, 0);
  analogWrite(ena_r, 0);
  digitalWrite(dir_l, LOW);
  digitalWrite(dir_r, LOW);
}

void shutdown_motors()
{
  digitalWrite(mode, LOW);
  analogWrite(ena_l, 0);
  analogWrite(ena_r, 0);
  digitalWrite(dir_l, LOW);
  digitalWrite(dir_r, LOW);
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

// TODO - update steering to some form of speed up & slow down combo, maybe even reverse slowed wheel
void drive_motors(int gear, int steer, int speed)
{
  l_speed = 0;
  r_speed = 0;

  if (gear == 2) // inplace
  {
    Serial.println("Inplace");
    l_speed =  abs(INP_L_BUFF * speed * (steer / steer_max));
    r_speed =  abs(INP_R_BUFF * speed * (steer / steer_max));

    if (steer == 0) // sit still
    {
      Serial.println("Stopped");
      l_speed =  0;
      r_speed =  0;
    }
    else if (steer > 0) // turn right (clockwise from top) in place
    {
      Serial.println("Clockwise");
      digitalWrite(dir_l, LOW);
      digitalWrite(dir_r, LOW);
    }
    else // turn left (counter-clockwise from top) in place
    {
      Serial.println("Counter-clockwise");
      digitalWrite(dir_l, HIGH);
      digitalWrite(dir_r, HIGH);
    }
  }
  else if (gear == 0) // forward
  {
    digitalWrite(dir_l, LOW);
    digitalWrite(dir_r, HIGH);
    if (steer >= 0) // steer right or straight
    {
      l_speed = FWD_L_BUFF * speed * (1 + abs(steer)/steer_max);
      r_speed = FWD_R_BUFF * (speed - REV_MAX) * (1 - abs(steer)/steer_max) + (FWD_R_BUFF * REV_MAX);
    }
    else // steer left
    {
      l_speed = FWD_L_BUFF * (speed - REV_MAX) * (1 - abs(steer)/steer_max) + (FWD_L_BUFF * REV_MAX);
      r_speed = FWD_R_BUFF * speed * (1 + abs(steer)/steer_max);
    }
    if (l_speed < 0)
    {
      digitalWrite(dir_l, HIGH);
      l_speed = l_speed * -1.0;
    }
    if (r_speed < 0)
    {
      digitalWrite(dir_r, LOW);
      r_speed = r_speed * -1.0;
    }
  }
  else if (gear == 1) // reverse
  {
    Serial.println("Reverse");
    digitalWrite(dir_l, HIGH);
    digitalWrite(dir_r, LOW);

    if (steer == 0) // drive straight
    {
      l_speed = REV_L_BUFF * speed;
      r_speed = REV_R_BUFF * speed;
    }
    else if (steer > 0) // steer right
    {
      l_speed = REV_L_BUFF * speed;
      r_speed = REV_R_BUFF * speed * (1 - abs(steer)/steer_max);
    }
    else // steer left
    {
      l_speed = REV_L_BUFF * speed * (1 - abs(steer)/steer_max);
      r_speed = REV_R_BUFF * speed;
    }
  }
  else
  {
    Serial.println("WARNING: Unknown gear value inputted: ");
    Serial.println(gear);
    Serial.println("Stopping motors");
  }

  // confirm speeds dont exceed bounds
  if (l_speed < 0)
    l_speed = 0;
  else if (l_speed > 255)
    l_speed = 255;
  if (r_speed < 0)
    r_speed = 0;
  else if (r_speed > 255)
    r_speed = 255;

  analogWrite(ena_l, l_speed);
  analogWrite(ena_r, r_speed);
}
