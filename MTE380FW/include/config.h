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

#define MOTOR_MODE PB3;  // MODE
#define MOTOR_ENA_L PB4; // ENB (PWM)
#define MOTOR_L PB5;     // INB
#define MOTOR_ENA_R PC7; // ENA (PWM)
#define MOTOR_R PA9;     // INA

// Consts that control system behaviour

#define SERVO_UP 180;  // servo input angle for up position
#define SERVO_DOWN 80; // servo input angle for down position

#define MOTOR_STEER 100.0; // granularity of steering inputs
#define MOTOR_SPEED 100;   // 0 to 255