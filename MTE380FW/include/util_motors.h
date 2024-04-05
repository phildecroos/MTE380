#include <Arduino.h>
#include "pinout.h"

#define L_FWD LOW
#define L_REV HIGH
#define R_FWD HIGH
#define R_REV LOW

const int MOTOR_MODE = PB5;   // D4, MODE
const int MOTOR_ENA_L = PC7;  // D9, ENB (PWM)
const int MOTOR_DIR_L = PA9;  // D8, INB
const int MOTOR_ENA_R = PA7;  // D11, ENA (PWM)
const int MOTOR_DIR_R = PA10; // D2, INA

enum drive_gears
{
  forward = 0,
  reverse = 1,
  inplace = 2
};

float fwd_scale_up = 0.0;
float fwd_scale_dn = 75.0;
float rev_scale_up = 0.0;
float rev_scale_dn = 50.0;

float l_speed, r_speed;

const float L_BUFF = 1.0;
const float R_BUFF = 1.0;

void setup_motors()
{
  pinMode(MOTOR_MODE, OUTPUT);
  pinMode(MOTOR_ENA_L, OUTPUT);
  pinMode(MOTOR_ENA_R, OUTPUT);
  pinMode(MOTOR_DIR_L, OUTPUT);
  pinMode(MOTOR_DIR_R, OUTPUT);

  digitalWrite(MOTOR_MODE, HIGH);
  analogWrite(MOTOR_ENA_L, 0);
  analogWrite(MOTOR_ENA_R, 0);
  digitalWrite(MOTOR_DIR_R, LOW);
  digitalWrite(MOTOR_DIR_R, LOW);
}

void shutdown_motors()
{
  digitalWrite(MOTOR_MODE, LOW);
  analogWrite(MOTOR_ENA_L, 0);
  analogWrite(MOTOR_ENA_R, 0);
  digitalWrite(MOTOR_DIR_L, LOW);
  digitalWrite(MOTOR_DIR_R, LOW);
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
    digitalWrite(MOTOR_DIR_L, L_FWD);
  else
    digitalWrite(MOTOR_DIR_L, L_REV);
  if (r_speed >= 0)
    digitalWrite(MOTOR_DIR_R, R_FWD);
  else
    digitalWrite(MOTOR_DIR_R, R_REV);

  analogWrite(MOTOR_ENA_L, abs(l_speed));
  analogWrite(MOTOR_ENA_R, abs(r_speed));
}
