#include "Adafruit_TCS34725.h"

// https://components101.com/sites/default/files/component_pin/STM32-Nucleo-F401RE-Pinout.png

/*
Pins used for colour sensors I2C:
PA8  (I2C3_SCL) Left
PC9  (I2C3_SDA) Left
PB8  (I2C1_SCL) Right
PB9  (I2C1_SDA) Right
PB10 (I2C2_SCL) Third
PB3  (I2C2_SDA) Third
*/

#define SERVO PB6; // PWM

#define MOTOR_MODE PB5;   // MODE
#define MOTOR_ENA_L PC7;  // ENB (PWM)
#define MOTOR_DIR_L PA9;  // INB
#define MOTOR_ENA_R PA7;  // ENA (PWM)
#define MOTOR_DIR_R PA10; // INA

#define SERVO_UP 0;    // servo input angle for up position
#define SERVO_DOWN 80; // servo input angle for down position

#define MOTOR_SPEED 125; // base speed of motors (0 to 255), reliable 125