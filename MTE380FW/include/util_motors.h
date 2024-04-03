#include <Arduino.h>
#include "config.h"

enum drive_gears
{
  forward = 0,
  reverse = 1,
  inplace = 2
};

int mode = MOTOR_MODE;
int ena_l = MOTOR_ENA_L;
int dir_l = MOTOR_DIR_L;
int ena_r = MOTOR_ENA_R;
int dir_r = MOTOR_DIR_R;
float fwd_scale_up = 0.0;
float fwd_scale_dn = 75.0;
float rev_scale_up = 0.0;
float rev_scale_dn = 50.0;

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

float clamp(float val, float min, float max)
{
  if (val > max)
    return max;
  else if (val < min)
    return min;
  return val;
}

void drive_motors(int gear, float steer, int speed)
{
  l_speed = 0;
  r_speed = 0;

  if (gear == inplace)
  {
    l_speed = L_BUFF * abs(L_BUFF * speed * abs(steer));
    r_speed = R_BUFF * abs(R_BUFF * speed * abs(steer));

    if (steer > 0) // CW
      r_speed *= -1;
    else // CCW
      l_speed *= -1;
  }
  else if (gear == forward)
  {
    if (steer > 0) // R
    {
      l_speed = L_BUFF * (speed + fwd_scale_up * abs(steer));
      r_speed = R_BUFF * (speed - fwd_scale_dn * abs(steer));
    }
    else // L
    {
      l_speed = L_BUFF * (speed - fwd_scale_dn * abs(steer));
      r_speed = R_BUFF * (speed + fwd_scale_up * abs(steer));
    }
  }
  else if (gear == reverse)
  {
    if (steer > 0) // R
    {
      l_speed = -1 * L_BUFF * (speed + rev_scale_dn * abs(steer));
      r_speed = -1 * R_BUFF * (speed - rev_scale_up * abs(steer));
    }
    else // L
    {
      l_speed = -1 * L_BUFF * (speed - rev_scale_up * abs(steer));
      r_speed = -1 * R_BUFF * (speed + rev_scale_dn * abs(steer));
    }
  }

  l_speed = clamp(l_speed, -255, 255);
  r_speed = clamp(r_speed, -255, 255);

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
