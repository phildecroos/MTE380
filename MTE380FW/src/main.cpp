#include "config.h"
#include "util_colour.h"
#include "util_servo.h"
#include "util_motors.h"
#include "util_ultrasonic.h"

const int UP = SERVO_UP;
const int DOWN = SERVO_DOWN;
const int SPEED = MOTOR_SPEED;
const float STEER_MAX = MOTOR_STEER;
const float MAX_LF_STEER = 1.0;

enum gears
{
  forward = 0,
  reverse = 1,
  inplace = 2
};

enum red_rgb_r // apprx readings of right colour sensor when fully on red line
{
  r_r_r = 810,
  g_r_r = 210,
  b_r_r = 150
};

enum red_rgb_l // apprx readings of left colour sensor when fully on red line
{
  r_r_l = 870,
  g_r_l = 210,
  b_r_l = 160
};

const float MAX_READING = 1000.0;
const int R_W_L = 1;
const int G_W_L = 1;
const int B_W_L = 1;
const int R_W_R = 1;
const int G_W_R = 1;
const int B_W_R = 1;

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

void demoDriveToStop()
{
  drive_motors(forward, 0, SPEED);
  int us_reading = 6;
  while (us_reading > 5)
  {
    us_reading = read_ultrasonic();
    Serial.println("US Reading: ");
    Serial.println(us_reading);
  }
  drive_motors(forward, 0, 0);
}

void demoGate()
{
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

void printCalibrationData()
{
  ColourReading col_in = read_colour();

  Serial.print(col_in.r_l);
  Serial.print(",");
  Serial.print(col_in.g_l);
  Serial.print(",");
  Serial.print(col_in.b_l);
  Serial.print("\n");

  // Serial.print(col_in.r_r);
  // Serial.print(",");
  // Serial.print(col_in.g_r);
  // Serial.print(",");
  // Serial.print(col_in.b_r);
  // Serial.print("\n");
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

// RGB weighted delta algorithm
float followAlgorithm(ColourReading col_in)
{
  float L_notred = abs((R_W_L * (col_in.r_l - r_r_l)) + (G_W_L * (col_in.g_l - g_r_l)) + (B_W_L * (col_in.b_l - b_r_l)));
  float R_notred = abs((R_W_R * (col_in.r_r - r_r_r)) + (G_W_R * (col_in.g_r - g_r_r)) + (B_W_R * (col_in.b_r - b_r_r)));
  float delta = L_notred - R_notred;

  Serial.println("Delta: ");
  Serial.println(delta);

  float max_delta = 1;
  if ((R_W_L + G_W_L + B_W_L) > (R_W_R + G_W_R + B_W_R))
  {
    max_delta = (R_W_L + G_W_L + B_W_L) * MAX_READING;
  }
  else
  {
    max_delta = (R_W_R + G_W_R + B_W_R) * MAX_READING;
  }
  float steering = ((STEER_MAX * MAX_LF_STEER) / max_delta) * delta;

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
  while (1)
  {
    lineFollow();
  }

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
