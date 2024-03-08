#include <Arduino.h>
#include "config.h"

int ena_l = MOTOR_ENA_L;
int for_l = MOTOR_F_L;
int back_l = MOTOR_B_L;
int ena_r = MOTOR_ENA_R;
int for_r = MOTOR_F_R;
int back_r = MOTOR_B_R;
float steer_max = MOTOR_STEER;

float l_speed;
float r_speed;

// do not make any buffs > 1 or that wheel will drive faster than set speed
const float INP_L_BUFF = 0.9;
const float INP_R_BUFF = 1.0;
const float FWD_L_BUFF = 0.9;
const float FWD_R_BUFF = 1.0;
const float REV_L_BUFF = 0.9;
const float REV_R_BUFF = 1.0;

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
      digitalWrite(for_l, LOW);
      digitalWrite(back_l, LOW);
      digitalWrite(for_r, LOW);
      digitalWrite(back_r, LOW);
    }
    else if (steer > 0) // turn right (clockwise from top) in place
    {
      Serial.println("Clockwise");
      digitalWrite(for_l, HIGH);
      digitalWrite(back_l, LOW);
      digitalWrite(for_r, LOW);
      digitalWrite(back_r, HIGH);
    }
    else // turn left (counter-clockwise from top) in place
    {
      Serial.println("Counter-clockwise");
      digitalWrite(for_l, LOW);
      digitalWrite(back_l, HIGH);
      digitalWrite(for_r, HIGH);
      digitalWrite(back_r, LOW);
    }
  }
  else if (gear == 0) // forward
  {
    Serial.println("Forward");
    digitalWrite(for_l, HIGH);
    digitalWrite(back_l, LOW);
    digitalWrite(for_r, HIGH);
    digitalWrite(back_r, LOW);

    if (steer == 0) // drive straight
    {
      l_speed = FWD_L_BUFF * speed;
      r_speed = FWD_R_BUFF * speed;
    }
    else if (steer > 0) // steer right
    {
      l_speed = FWD_L_BUFF * speed;
      r_speed = (FWD_R_BUFF * speed * ((steer_max - steer) / steer_max));
    }
    else // steer left
    {
      l_speed = (FWD_L_BUFF * speed * ((steer_max - abs(steer)) / steer_max));
      r_speed = FWD_R_BUFF * speed;
    }
  }
  else if (gear == 1) // reverse
  {
    Serial.println("Reverse");
    digitalWrite(for_l, LOW);
    digitalWrite(back_l, HIGH);
    digitalWrite(for_r, LOW);
    digitalWrite(back_r, HIGH);

    if (steer == 0) // drive straight
    {
      l_speed = REV_L_BUFF * speed;
      r_speed = REV_R_BUFF * speed;
    }
    else if (steer > 0) // steer right
    {
      l_speed = REV_L_BUFF * speed;
      r_speed = (REV_R_BUFF * speed * ((steer_max - steer) / steer_max));
    }
    else // steer left
    {
      l_speed = (REV_L_BUFF * speed * ((steer_max - abs(steer)) / steer_max));
      r_speed = REV_R_BUFF * speed;
    }
  }
  else
  {
    Serial.println("WARNING: Unknown gear value inputted: ");
    Serial.println(gear);
    Serial.println("Stopping motors");
  }

  Serial.println("L speed: ");
  Serial.println(l_speed);
  Serial.println("R speed: ");
  Serial.println(r_speed);
  analogWrite(ena_l, l_speed);
  analogWrite(ena_r, r_speed);
}
