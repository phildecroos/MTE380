#include "config.h"
#include "util_colour.h"
#include "util_servo.h"
#include "util_motors.h"
#include "util_ultrasonic.h"

const int UP = SERVO_UP;
const int DOWN = SERVO_DOWN;
const int SPEED = MOTOR_SPEED;
const int STEER_MAX = MOTOR_STEER;

enum gears
{
  forward = 0,
  reverse = 1,
  inplace = 2
};

/*
24ms

gain 1x
r 50
g 12
b 10

gain 4x
r 200
g 47
b 36

gain 16x
r 800
g 190
b 145

gain 60x
r 3100
g 715
b 550
*/

// TODO - calibrate these enums to the values of the actual course
const int K_P = 1;
const int R_W_L = 1;
const int G_W_L = 1;
const int B_W_L = 1;
const int R_W_R = 1;
const int G_W_R = 1;
const int B_W_R = 1;
float max_diff = 0;

enum red_rgb_r // apprx readings of colour sensors when fully on red line
{
  r_r_r = 810,
  g_r_r = 210,
  b_r_r = 150
};

enum red_rgb_l // apprx readings of colour sensors when fully on red line
{
  r_r_l = 870,
  g_r_l = 210,
  b_r_l = 160
};

// TODO - Demo program to show all driving functions
void demoDrive()
{
  drive_motors(forward, 0, SPEED);
  delay(3000);
  drive_motors(reverse, 0, SPEED);
  delay(3000);
  drive_motors(forward, 40, SPEED);
  delay(2000);
  drive_motors(reverse, -40, SPEED);
  delay(2000);
  drive_motors(inplace, 100, SPEED);
  delay(2000);
  drive_motors(inplace, -100, SPEED);
  delay(2000);
}

void demoGate() {
  int pos = 0;
  Serial.println("DOWN");
  move_servo(DOWN);
  while (1)
  {
    int us_reading = read_ultrasonic();
    Serial.println("US Reading: ");
    Serial.println(us_reading);
    if (us_reading < 5)
    {
      if (pos == 0)
      {
        Serial.println("UP");
        move_servo(UP);
        pos = 1;
      }
      else
      {
        Serial.println("DOWN");
        move_servo(DOWN);
        pos = 0;
      }
      delay(1000);
    }
  }
}

void readColours()
{
  ColourReading col_in = read_colour();

  Serial.print("\n---------------------\n");

  Serial.print("R Left: ");
  Serial.print(col_in.r_l, DEC);
  Serial.print(" ");
  Serial.print("G Left: ");
  Serial.print(col_in.g_l, DEC);
  Serial.print(" ");
  Serial.print("B Left: ");
  Serial.print(col_in.b_l, DEC);
  Serial.print(" ");
  Serial.println(" ");

  Serial.print("R Right: ");
  Serial.print(col_in.r_r, DEC);
  Serial.print(" ");
  Serial.print("G Right: ");
  Serial.print(col_in.g_r, DEC);
  Serial.print(" ");
  Serial.print("B Right: ");
  Serial.print(col_in.b_r, DEC);
  Serial.print(" ");
  Serial.println(" ");
}

// Line following control algorithm
float followAlgorithm(ColourReading col_in)
{
  float L_notred = abs(R_W_L * (col_in.r_l - r_r_l) + G_W_L * (col_in.g_l - g_r_l) + B_W_L * (col_in.b_l - b_r_l));
  float R_notred = abs(R_W_R * (col_in.r_r - r_r_r) + G_W_R * (col_in.g_r - g_r_r) + B_W_R * (col_in.b_r - b_r_r));
  float diff = L_notred - R_notred;

  Serial.println("Diff: ");
  Serial.println(diff);

  float steering = (70.0 / 2500.0) * diff;
  Serial.println("steering: ");
  Serial.println(steering);

  return steering;
}

// Line following program
void lineFollow()
{
  ColourReading col_in = read_colour();

  Serial.print("\n---------------------\n");

  Serial.print("R Left: ");
  Serial.print(col_in.r_l, DEC);
  Serial.print(" ");
  Serial.print("G Left: ");
  Serial.print(col_in.g_l, DEC);
  Serial.print(" ");
  Serial.print("B Left: ");
  Serial.print(col_in.b_l, DEC);
  Serial.print(" ");
  Serial.println(" ");

  Serial.print("R Right: ");
  Serial.print(col_in.r_r, DEC);
  Serial.print(" ");
  Serial.print("G Right: ");
  Serial.print(col_in.g_r, DEC);
  Serial.print(" ");
  Serial.print("B Right: ");
  Serial.print(col_in.b_r, DEC);
  Serial.print(" ");
  Serial.println(" ");

  int steering = followAlgorithm(col_in);
  drive_motors(forward, steering, SPEED);
  Serial.println("steering: ");
  Serial.println(steering);
}

// TODO - make pickup routine
void pickUp()
{
}

// TODO - make dropoff routine
void dropOff()
{
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Setting up...");
  setup_ultrasonic();
  Serial.println("Set up ultrasonic sensor");
  setup_servo();
  Serial.println("Set up servo motor");
  setup_motors();
  Serial.println("Set up dc motors");
  setup_colour();
  Serial.println("Set up colour sensors");
  Serial.println("Setup complete");
}

// TODO - set up flow for overall process
void loop()
{
  demoDrive();

  Serial.println("Shutting down...");
  shutdown_motors();
  Serial.println("Shut down dc motors");
  shutdown_servo();
  Serial.println("Shut down servo motor");
  shutdown_colour();
  Serial.println("Shut down colour sensors");
  shutdown_ultrasonic();
  Serial.println("Shut down ultrasonic sensor");
  Serial.println("Shutdown complete");

  while (1)
    ;
}
