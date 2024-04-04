#include "Adafruit_TCS34725.h"

// https://components101.com/sites/default/files/component_pin/STM32-Nucleo-F401RE-Pinout.png

#define TCSL_SDA PC9;  // Top pin, All I2C
#define TCSL_SCL PA8;  // D7
#define TCSR_SDA PB9;  // D14
#define TCSR_SCL PB8;  // D15
#define TCS3_SDA PB3;  // D3
#define TCS3_SCL PB10; // D6

#define SERVO PB6; // D10, PWM

#define MOTOR_MODE PB5;   // D4, MODE
#define MOTOR_ENA_L PC7;  // D9, ENB (PWM)
#define MOTOR_DIR_L PA9;  // D8, INB
#define MOTOR_ENA_R PA7;  // D11, ENA (PWM)
#define MOTOR_DIR_R PA10; // D2, INA
