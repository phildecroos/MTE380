#include "general.h"
#include "util_colour.h"
#include "util_servo.h"
#include "util_motors.h"

enum lf_exit_cases
{
  bullseye = 0,
  safezone = 1,
  returned = 2,
  returnll = 3,
  returnlr = 4,
  returnl3 = 5,
  waittime = 6
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

const float G_START_R = 150.0; // green threshold for both sensors to see tape at start
const float G_START_L = 150.0;

bool check_exit(int exit_case, ColourReading col_in)
{
  return (
     ((exit_case == bullseye) && (col_in.b_l > B_BULLSEYE_L) && (col_in.b_r > B_BULLSEYE_R) && (col_in.b_3 > B_BULLSEYE_3))
  || ((exit_case == returnl3) && (col_in.g_3 < G_RED_3))
  || ((exit_case == returnll) && (col_in.g_l < (G_RED_L + 15)))
  || ((exit_case == returnlr) && (col_in.g_r < (G_RED_R + 15)))
  || ((exit_case == safezone) && (col_in.r_3 < R_SAFEZONE_3) && (col_in.g_3 < G_SAFEZONE_3) && (col_in.b_3 < B_SAFEZONE_3))
  || ((exit_case == returned) && (col_in.g_l < (G_START_L)) && (col_in.g_r < (G_START_R)))
  );
}

void wait_until(int exit_case, int exit_samples)
{
  bool prev[exit_samples] = {0};
  while (1)
  {
    shift(prev, exit_samples);
    prev[0] = check_exit(exit_case, read_colour());
    if (check_all(prev, exit_samples))
      break;
  }
}

float follow_algorithm(int exit_case, float Kp, ColourReading col_in)
{
  if (check_exit(exit_case, col_in))
    return 2.0;

  float L_error = clamp(((col_in.g_l - G_RED_L) / (G_WOOD_L - G_RED_L)), 0.0, 1.0);
  float R_error = clamp(((col_in.g_r - G_RED_R) / (G_WOOD_R - G_RED_R)), 0.0, 1.0);
  float E = clamp((L_error - R_error), -1.0, 1.0);

  float steering = Kp * pow(abs(E), 4); // reliable pow 4

  if (E < 0)
    steering *= -1.0;
  return clamp(steering, -1.0, 1.0);
}

void line_follow(int speed, float Kp, int exit_case, int exit_samples)
{
  if (exit_case == waittime)
  {
    for (int i = 0; i < exit_samples; i++)
    {
      drive_motors(forward, follow_algorithm(exit_case, Kp, read_colour()), speed);
    }
  }
  else
  {
    bool prev[exit_samples] = {0};
    while (1)
    {
      shift(prev, exit_samples);

      float steering = follow_algorithm(exit_case, Kp, read_colour());
      if (steering != 2.0)
        drive_motors(forward, steering, speed);
      prev[0] = (steering == 2.0);

      if (check_all(prev, exit_samples))
        break;
    }
  }
}

void pick_up()
{
  drive_motors(inplace, 1.0, 50);
  delay(100);

  move_servo(DOWN);
  drive_motors(forward, 0.0, 50);
  delay(200);

  drive_motors(forward, 0.0, 0);
  delay(100);

  drive_motors(inplace, 1.0, 100);
  delay(700);
  wait_until(returnll, 3);

  drive_motors(inplace, -1.0, 100);
  delay(250);

  drive_motors(forward, 0.0, 0);
}

void drop_off()
{
  drive_motors(inplace, 1.0, 50);
  delay(300);

  drive_motors(forward, 0.0, 50);
  delay(700);

  drive_motors(reverse, 0.2, 50);
  move_servo(UP);
  delay(2300);

  drive_motors(forward, 1.0, 100);
  wait_until(returnll, 3);

  drive_motors(forward, -1.0, 100);
  wait_until(returnlr, 3);

  drive_motors(forward, 0.0, 0);
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Setting up...");
  setup_motors();
  setup_servo();
  if (!setup_colour())
  {
    Serial.println("TCS not found");
    while (1);
  }
  Serial.println("Setup complete");
}

void loop()
{
  Serial.println("Line following for the first straight");
  line_follow(255, 1.1, waittime, 240); // ~71 exit_samples/second, reliable 255, 1.1
  Serial.println("Line following for the first turn");
  line_follow(125, 2.0, waittime, 230); // reliable 125, 2.0 everywhere
  Serial.println("Line following for the second straight");
  line_follow(255, 1.1, waittime, 180);
  Serial.println("Line following to the bullseye");
  line_follow(125, 2.0, bullseye, 1);
  Serial.println("Picking up minifigure");
  pick_up();
  Serial.println("Line following to get back to the line");
  line_follow(110, 2.0, waittime, 50);
  Serial.println("Line following to the safe zone");
  line_follow(125, 2.0, safezone, 3);
  Serial.println("Dropping off minifigure");
  drop_off();
  Serial.println("Line following back to start");
  line_follow(125, 2.0, returned, 3);

  Serial.println("Shutting down...");
  shutdown_motors();
  shutdown_servo();
  Serial.println("Shutdown complete");

  while (1);
}
