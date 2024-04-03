#include "config.h"
#include "util_colour.h"
#include "util_servo.h"
#include "util_motors.h"
#include "util_ultrasonic.h"

const int UP_POS = SERVO_UP;
const int DOWN_POS = SERVO_DOWN;
const int DRIVE_SPEED = MOTOR_SPEED;
const int HISTORY = 50;

enum lf_exit_cases
{
  bullseye = 0,
  returnln = 1,
  safezone = 2,
  returned = 3
};

const float G_RED_R = 120.0; // calibrated green readings for red tape and wood
const float G_WOOD_R = 400.0;
const float G_RED_L = 100.0;
const float G_WOOD_L = 300.0;

const float B_BULLSEYE_R = 200.0; // calibrated blue readings for the bullseye
const float B_BULLSEYE_L = 200.0;
const float B_BULLSEYE_3 = 600.0;
const float G_RED_3 = 300.0; // calibrated green reading for red tape

const float R_SAFEZONE_3 = 300.0; // calibrated readings on dark green tape of safe zone
const float G_SAFEZONE_3 = 300.0;
const float B_SAFEZONE_3 = 300.0;

const float G_START_R = 150.0; // calibrated green readings for red tape at start
const float G_START_L = 150.0;

void printCalibrationData()
{
  ColourReading col_in = read_colour();

  // Serial.print("R: ");
  // Serial.print(col_in.r_l);
  // Serial.print(",");
  // Serial.print(col_in.r_r);
  // Serial.print(",");
  // Serial.print(col_in.r_3);
  // Serial.print("\n");

  // Serial.print("G: ");
  // Serial.print(col_in.g_l);
  // Serial.print(",");
  // Serial.print(col_in.g_r);
  // Serial.print(",");
  // Serial.print(col_in.g_3);
  // Serial.print("\n");

  Serial.print("B: ");
  Serial.print(col_in.b_l);
  Serial.print(",");
  Serial.print(col_in.b_r);
  Serial.print(",");
  Serial.print(col_in.b_3);
  Serial.print("\n");

  // Serial.print("Left:");
  // Serial.print(col_in.r_l);
  // Serial.print(",");
  // Serial.print(col_in.g_l);
  // Serial.print(",");
  // Serial.print(col_in.b_l);
  // Serial.print("\n");

  // Serial.print("Right:");
  // Serial.print(col_in.r_r);
  // Serial.print(",");
  // Serial.print(col_in.g_r);
  // Serial.print(",");
  // Serial.print(col_in.b_r);
  // Serial.print("\n");

  // Serial.print("Three:");
  // Serial.print(col_in.r_3);
  // Serial.print(",");
  // Serial.print(col_in.g_3);
  // Serial.print(",");
  // Serial.print(col_in.b_3);
  // Serial.print("\n");

  delay(250);
}

bool checkExit(int exit_case, ColourReading col_in)
{
  if ((exit_case == bullseye) && (col_in.b_l > B_BULLSEYE_L) && (col_in.b_r > B_BULLSEYE_R) && (col_in.b_3 > B_BULLSEYE_3))
    return true;
  if ((exit_case == returnln) && (col_in.g_3 < G_RED_3))
    return true;
  if ((exit_case == safezone) && (col_in.r_3 < R_SAFEZONE_3) && (col_in.g_3 < G_SAFEZONE_3) && (col_in.b_3 < B_SAFEZONE_3))
    return true;
  if ((exit_case == returned) && (col_in.g_l < (G_START_L)) && (col_in.g_r < (G_START_R)))
    return true;
  return false;
}

void waitUntil(int exit_case, int exit_samples)
{
  bool prev[exit_samples] = {0};
  while (1)
  {
    for (int i = 1; i < exit_samples; i++)
    {
      prev[exit_samples - i] = prev[exit_samples - i - 1];
    }

    prev[0] = checkExit(exit_case, read_colour());

    bool stop = true;
    for (int i = 0; i < exit_samples; i++)
    {
      if (!prev[i])
      {
        stop = false;
        break;
      }
    }
    if (stop)
      break;
  }
}

float followAlgorithm(int exit_case, ColourReading col_in, float *prev_steer)
{
  if (checkExit(exit_case, col_in))
    return 2;

  float L_error = clamp(((col_in.g_l - G_RED_L) / (G_WOOD_L - G_RED_L)), 0.0, 1.0);
  float R_error = clamp(((col_in.g_r - G_RED_R) / (G_WOOD_R - G_RED_R)), 0.0, 1.0);
  float E = clamp((L_error - R_error), -1.0, 1.0);

  float Kp = 2.0;                       // reliable 2.0
  float steering = Kp * pow(abs(E), 4); // reliable pow 4

  if (E < 0.0)
    steering *= -1.0;
  steering = clamp(steering, -1.0, 1.0);
  return steering;
}

void lineFollow(int exit_case, int exit_samples)
{
  float prev_steer[HISTORY] = {0};
  while (1)
  {
    for (int i = 1; i < HISTORY; i++)
    {
      prev_steer[HISTORY - i] = prev_steer[HISTORY - i - 1];
    }

    float steering = followAlgorithm(exit_case, read_colour(), prev_steer);
    if (steering != (2))
      drive_motors(forward, steering, DRIVE_SPEED);
    prev_steer[0] = steering;

    bool stop = true;
    for (int i = 0; i < exit_samples; i++)
    {
      if (prev_steer[i] != (2))
        stop = false;
    }
    if (stop)
      break;
  }
}

void pickUp()
{
  drive_motors(inplace, 1.0, 50);
  delay(100);

  move_servo(DOWN_POS);
  drive_motors(forward, 0.0, 50);
  delay(300);

  drive_motors(inplace, 1.0, 75);
  delay(750);
  waitUntil(returnln, 3);
  delay(200);

  drive_motors(forward, 0.0, 0);
}

void dropOff()
{
  drive_motors(inplace, 1.0, 50);
  delay(200);
  waitUntil(safezone, 3);
  delay(100);

  drive_motors(forward, 0.0, 50);
  delay(700);

  drive_motors(reverse, 0.0, 50);
  move_servo(UP_POS);
  delay(500);

  drive_motors(inplace, -1.0, 50);
  delay(200);

  drive_motors(reverse, 0.25, 50);
  delay(1200);

  drive_motors(forward, 0.0, 0);
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

void loop()
{
  Serial.println("Line following to the bullseye");
  lineFollow(bullseye, 1);
  Serial.println("Picking up minifigure");
  pickUp();
  Serial.println("Line following to the safe zone");
  lineFollow(safezone, 10);
  Serial.println("Dropping off minifigure");
  dropOff();
  Serial.println("Line following back to start");
  lineFollow(returned, 3);

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
