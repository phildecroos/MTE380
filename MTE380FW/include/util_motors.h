#include <Arduino.h>
#include "config.h"

void setup_motors() {
  int ena_l = MOTOR_ENA_L;
  // int for_l = MOTOR_F_L;
  // int back_l = MOTOR_B_L;
  // int ena_r = MOTOR_ENA_R;
  // int for_r = MOTOR_F_R;
  // int back_r = MOTOR_B_R;

  pinMode(ena_l, OUTPUT);
  // pinMode(for_l, OUTPUT);
  // pinMode(back_l, OUTPUT);
  // pinMode(ena_r, OUTPUT);
  // pinMode(for_r, OUTPUT);
  // pinMode(back_r, OUTPUT);
}

// TODO - add driving/steering logic here
void drive_motors(int in) {
  int ena_l = MOTOR_ENA_L;
  // int for_l = MOTOR_F_L;
  // int back_l = MOTOR_B_L;
  // int ena_r = MOTOR_ENA_R;
  // int for_r = MOTOR_F_R;
  // int back_r = MOTOR_B_R;

  // digitalWrite(for_l, HIGH);
  // digitalWrite(back_l, LOW);
  // digitalWrite(for_r, HIGH);
  // digitalWrite(back_r, LOW);
  if (in == 1) {
    analogWrite(ena_l, 250);
  }
  else {
    analogWrite(ena_l, 50);
  }
  // analogWrite(ena_r, 100);
}