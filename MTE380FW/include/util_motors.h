#include <Arduino.h>
#include "config.h"

void setup_motors() {
  int ena_l = MOTOR_ENA_L;
  int for_l = MOTOR_F_L;
  int back_l = MOTOR_B_L;
  int ena_r = MOTOR_ENA_R;
  int for_r = MOTOR_F_R;
  int back_r = MOTOR_B_R;

  pinMode(ena_l, OUTPUT);
  pinMode(for_l, OUTPUT);
  pinMode(back_l, OUTPUT);
  pinMode(ena_r, OUTPUT);
  pinMode(for_r, OUTPUT);
  pinMode(back_r, OUTPUT);
}

void drive_motors(bool follow, int steer, int speed) {
  int ena_l = MOTOR_ENA_L;
  int for_l = MOTOR_F_L;
  int back_l = MOTOR_B_L;
  int ena_r = MOTOR_ENA_R;
  int for_r = MOTOR_F_R;
  int back_r = MOTOR_B_R;
  int steer_max = MOTOR_STEER;

  if (follow) {
    digitalWrite(for_l, HIGH);
    digitalWrite(back_l, LOW);
    digitalWrite(for_r, HIGH);
    digitalWrite(back_r, LOW);

    if (steer == 0) {
      analogWrite(ena_l, speed);
      analogWrite(ena_r, speed);
    }
    else if (steer > 0) {
      analogWrite(ena_l, speed);
      analogWrite(ena_r, (speed * (steer / steer_max)));
    }
    else {
      analogWrite(ena_l, (speed * (steer / steer_max)));
      analogWrite(ena_r, speed);
    }
  }

  // TODO - add other cases for driving not related to line following
}