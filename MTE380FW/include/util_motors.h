#include <Arduino.h>
#include "config.h"

int mode = MOTOR_MODE;
int ena_l = MOTOR_ENA_L;
int dir_l = MOTOR_DIR_L;
int ena_r = MOTOR_ENA_R;
int dir_r = MOTOR_DIR_R;
float fwd_scale = 50.0;
float rev_scale = 50.0;

#define L_FWD LOW
#define L_REV HIGH
#define R_FWD HIGH
#define R_REV LOW

float l_speed;
float r_speed;

const float L_BUFF = 1.0;
const float R_BUFF = 1.0;

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

void drive_motors(int gear, int steer, int speed)
{
  l_speed = 0;
  r_speed = 0;

  if (gear == 2) // inplace
  {
    l_speed = L_BUFF * abs(L_BUFF * speed * abs(steer));
    r_speed = R_BUFF * abs(R_BUFF * speed * abs(steer));

    if (steer > 0) // CW
      r_speed *= -1;
    else // CCW
      l_speed *= -1;
  }
  else if (gear == 0) // forward
  {
    if (steer > 0) // R
    {
      l_speed = L_BUFF * (speed + fwd_scale * abs(steer));
      r_speed = R_BUFF * (speed - fwd_scale * abs(steer));
    }
    else // L
    {
      l_speed = L_BUFF * (speed - fwd_scale * abs(steer));
      r_speed = R_BUFF * (speed + fwd_scale * abs(steer));
    }
  }
  else if (gear == 1) // reverse
  {
    if (steer > 0) // R
    {
      l_speed = -1 * L_BUFF * (speed + rev_scale * abs(steer));
      r_speed = -1 * R_BUFF * (speed - rev_scale * abs(steer));
    }
    else // L
    {
      l_speed = -1 * L_BUFF * (speed - rev_scale * abs(steer));
      r_speed = -1 * R_BUFF * (speed + rev_scale * abs(steer));
    }
  }

  if (l_speed > 255)
    l_speed = 255;
  else if (l_speed < -255)
    l_speed = -255;
  if (r_speed > 255)
    r_speed = 255;
  else if (r_speed < -255)
    r_speed = -255;

  if (l_speed >= 0)
    digitalWrite(dir_l, L_FWD);
  else
    digitalWrite(dir_l, L_REV);
  if (r_speed >= 0)
    digitalWrite(dir_r, R_FWD);
  else
    digitalWrite(dir_r, R_REV);
  analogWrite(ena_l, abs(l_speed));
  analogWrite(ena_r, abs(r_speed));
}
