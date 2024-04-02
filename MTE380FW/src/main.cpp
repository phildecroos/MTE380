#include "config.h"
#include "util_colour.h"
#include "util_servo.h"
#include "util_motors.h"
#include "util_ultrasonic.h"

const int UP_POS = SERVO_UP;
const int DOWN_POS = SERVO_DOWN;
const int DRIVE_SPEED = MOTOR_SPEED;
const int HISTORY = 50;

enum driving_gears
{
  forward = 0,
  reverse = 1,
  inplace = 2
};

enum lf_exit_cases
{
  bullseye = 0,
  safezone = 1,
  returned = 2
};

const float G_RED_R = 125.0; // calibrated green readings for red tape and wood
const float G_WOOD_R = 500.0;
const float G_RED_L = 95.0;
const float G_WOOD_L = 400.0;

const float B_BULLSEYE_R = 200.0; // calibrated blue readings for the bullseye
const float B_BULLSEYE_L = 200.0;

const float R_SAFEZONE_3 = 300.0; // calibrated readings on dark green tape of safe zone
const float G_SAFEZONE_3 = 300.0;
const float B_SAFEZONE_3 = 300.0;

void printCalibrationData()
{
  ColourReading col_in = read_colour();

  // Serial.print("G: ");
  // Serial.print(col_in.g_l);
  // Serial.print(",");
  // Serial.print(col_in.g_r);
  // Serial.print(",");
  // Serial.print(col_in.g_3);
  // Serial.print("\n");

  // Serial.print("B: ");
  // Serial.print(col_in.b_l);
  // Serial.print(",");
  // Serial.print(col_in.b_r);
  // Serial.print(",");
  // Serial.print(col_in.b_3);
  // Serial.print("\n");

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

  Serial.print("Three:");
  Serial.print(col_in.r_3);
  Serial.print(",");
  Serial.print(col_in.g_3);
  Serial.print(",");
  Serial.print(col_in.b_3);
  Serial.print("\n");

  delay(250);
}

float followAlgorithm(int exit_case, ColourReading col_in, float *prev_steer)
{
  if ((exit_case == bullseye) && (col_in.b_l > B_BULLSEYE_L) && (col_in.b_r > B_BULLSEYE_R))
    return 2;
  else if ((exit_case == safezone) && (col_in.r_3 < R_SAFEZONE_3) && (col_in.g_3 < G_SAFEZONE_3) && (col_in.b_3 < B_SAFEZONE_3))
    return 2;

  float L_notred = abs(col_in.g_l - G_RED_L);
  float R_notred = abs(col_in.g_r - G_RED_R);
  float delta = L_notred - R_notred;
  float delta_ratio = 0;
  if (delta > 0)
    delta_ratio = abs(delta / abs(G_WOOD_L - G_RED_L));
  else if (delta < 0)
    delta_ratio = abs(delta / abs(G_WOOD_R - G_RED_R));
  if (delta_ratio > 1.0)
    delta_ratio = 1.0;

  float Kp = 2.0;
  // float Kd_down = 5.0;

  float steering = Kp * pow(delta_ratio, 2);
  // if ((prev_steer[0] * delta > 0.0) && (abs(prev_steer[0]) > steering))
  //   steering = prev_steer[0] - Kd_down * (abs(prev_steer[0]) - steering);

  if (steering < 0.0)
    steering = 0.0;
  if (delta > 0.0)
    steering = -1.0 * steering;

  if (steering > 1)
    steering = 1;
  else if (steering < -1)
    steering = -1;
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
  drive_motors(forward, 0, 50);
  move_servo(DOWN_POS);
  delay(100);

  drive_motors(forward, 0, 0);
  delay(250);

  drive_motors(inplace, 100, 150);
  delay(500);

  drive_motors(forward, 0, 0);
}

void dropOff()
{
  drive_motors(forward, 0, 0);
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
  // while (1)
  // {
  //   printCalibrationData();
  // }

  lineFollow(bullseye, 10);
  pickUp();
  lineFollow(safezone, 10);
  dropOff();
  lineFollow(returned, 10);

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
