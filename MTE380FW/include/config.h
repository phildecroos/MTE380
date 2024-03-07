#include "Adafruit_TCS34725.h"

// Pin mapping

// https://components101.com/sites/default/files/component_pin/STM32-Nucleo-F401RE-Pinout.png

/*
Pins used for colour sensors I2C:
PB8 (I2C1_SCL)
PB9 (I2C1_SDA)
PA8 (I2C3_SCL)
PC9 (I2C3_SDA)
*/

#define US_TRIGGER PA5;
#define US_ECHO PA6;

#define START_BUTTON PB10;

#define SERVO PB6; // PWM

#define MOTOR_ENA_L PC7; // ENB (PWM)
#define MOTOR_B_L PA7;   // IN3
#define MOTOR_F_L PB5;   // IN4
#define MOTOR_ENA_R PB4; // ENA (PWM)
#define MOTOR_B_R PB3;   // IN2
#define MOTOR_F_R PA9;   // IN1

// Consts that control system behaviour

// TODO - test and update servo up and down input angles
#define SERVO_UP 0;   // servo input angle for up position
#define SERVO_DOWN 0; // servo input angle for down position

#define MOTOR_STEER 10;  // granularity of steering inputs
#define MOTOR_SPEED 80; // 0 to 255