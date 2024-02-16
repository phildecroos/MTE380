#include "Adafruit_TCS34725.h"

// Pin mapping

/*
Pins used for colour sensors I2C:
PB8
PB9
PC9
PA8
*/

// TODO - find pins to use for all uses below, and specify here
#define US_TRIGGER 10; // PWM?
#define US_ECHO 11; // PWM?

#define SERVO 10; // PWM

#define MOTOR_ENA_L 9; // PWM
#define MOTOR_F_L 7;
#define MOTOR_B_L 6;
#define MOTOR_ENA_R 4; // PWM
#define MOTOR_F_R 5;
#define MOTOR_B_R 8;

#define MOTOR_STEER 100; // granularity of steering inputs
#define MOTOR_SPEED 200; // 0 to 255