#include "Adafruit_TCS34725.h"

// https://components101.com/sites/default/files/component_pin/STM32-Nucleo-F401RE-Pinout.png

#define TCSL_SDA PC9; // All I2C
#define TCSL_SCL PA8;
#define TCSR_SDA PB9;
#define TCSR_SCL PB8;
#define TCS3_SDA PB3;
#define TCS3_SCL PB10;

#define SERVO PB6; // PWM

#define MOTOR_MODE PB5;   // MODE
#define MOTOR_ENA_L PC7;  // ENB (PWM)
#define MOTOR_DIR_L PA9;  // INB
#define MOTOR_ENA_R PA7;  // ENA (PWM)
#define MOTOR_DIR_R PA10; // INA
