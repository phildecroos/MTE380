#include "config.h"
#include "util_colour.h"
#include "util_servo.h"
#include "util_motors.h"
#include "util_ultrasonic.h"

const int UP_POS = SERVO_UP;
const int DOWN_POS = SERVO_DOWN;
const int DRIVE_SPEED = MOTOR_SPEED;
const float STEER_RESOLUTION = MOTOR_STEER;
const float MAX_LF_STEER = 1.0;

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

// TODO - update needed values to use R,G,B not just one of them
const float G_R_R = 90.0;  // calibrated green reading for right sensor on tape
const float G_W_R = 500.0; // calibrated green reading for right sensor on wood
const float B_B_R = 190.0; // calibrated green reading for right sensor on blue (bullseye)
const float G_R_L = 90.0;  // calibrated green reading for left sensor on tape
const float G_W_L = 500.0; // calibrated green reading for left sensor on wood
const float B_B_L = 190.0; // calibrated green reading for left sensor on blue (bullseye)
const float WEIGHTS[2] = {1.0, 1.0};

void demoDrive()
{
  drive_motors(forward, 0, DRIVE_SPEED);
  // delay(3000);
  // drive_motors(reverse, 0, DRIVE_SPEED);
  // delay(3000);
  // drive_motors(forward, 0, 0);
  // drive_motors(forward, 40, DRIVE_SPEED);
  // delay(2000);
  // drive_motors(reverse, -40, DRIVE_SPEED);
  // delay(2000);
  // drive_motors(inplace, 100, DRIVE_SPEED);
  // delay(2000);
  // drive_motors(inplace, -100, DRIVE_SPEED);
  // delay(2000);
}

void demoDriveToStop()
{
  drive_motors(forward, 0, DRIVE_SPEED);
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
  move_servo(DOWN_POS);
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
        move_servo(UP_POS);
        pos = 1;
      }
      else
      {
        Serial.println("DOWN");
        move_servo(DOWN_POS);
        pos = 0;
      }
      delay(1000);
    }
  }
}

void demoPickDrop()
{
  Serial.println("UP");
  move_servo(UP_POS);
  delay(500);

  drive_motors(forward, 0, DRIVE_SPEED);
  delay(1000);

  drive_motors(reverse, 0, 0);
  Serial.println("DOWN");
  move_servo(DOWN_POS);
  delay(500);

  drive_motors(reverse, 0, DRIVE_SPEED);
  delay(1000);

  drive_motors(reverse, 0, 0);
  delay(1000);

  drive_motors(forward, 0, DRIVE_SPEED);
  delay(1000);

  drive_motors(reverse, 0, 0);
  Serial.println("UP");
  move_servo(UP_POS);
  delay(500);

  drive_motors(reverse, 0, DRIVE_SPEED);
  delay(1000);
}

void printCalibrationData()
{
  ColourReading col_in = read_colour();

  // Serial.print(col_in.g_l);
  // Serial.print(",");
  // Serial.print(col_in.g_r);
  // Serial.print("\n");

  Serial.print("Left:");
  Serial.print(col_in.r_l);
  Serial.print(",");
  Serial.print(col_in.g_l);
  Serial.print(",");
  Serial.print(col_in.b_l);
  Serial.print("\n");

  Serial.print("Right:");
  Serial.print(col_in.r_r);
  Serial.print(",");
  Serial.print(col_in.g_r);
  Serial.print(",");
  Serial.print(col_in.b_r);
  Serial.print("\n");

  delay(500);
}

void printColours()
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

float followAlgorithm(int exit_case, ColourReading col_in, float prev_steer)
{
  // return (STEER_RESOLUTION + 1) if exit_case is met
  if ((exit_case == bullseye) && ((col_in.b_l > B_B_L) && (col_in.b_r > B_B_R)))
    return STEER_RESOLUTION + 1;
  // TODO - add checks for other exit cases

  // calculate error function (0 to 1, sign of delta is steer direction)
  float L_notred = WEIGHTS[0] * abs(col_in.g_l - G_R_L);
  float R_notred = WEIGHTS[1] * abs(col_in.g_r - G_R_R);
  float delta = L_notred - R_notred;
  float delta_ratio = 0;
  if (delta > 0)
    delta_ratio = abs(delta / (WEIGHTS[0] * abs(G_W_L - G_R_L)));
  else if (delta < 0)
    delta_ratio = abs(delta / (WEIGHTS[1] * abs(G_W_R - G_R_R)));
  if (delta_ratio > 1.0)
    delta_ratio = 1.0;
  // TODO - could add dead zone here where small enough delta_ratios are just forced to zero to increase dampening

  // determine gains
  // TODO - tune this for updated steering method & to dial in turning
  float Kp = 0.5;
  float Kd = 3.0;
  if (delta_ratio > 0.30)
    Kp = 3 * pow(delta_ratio, 1);

  // calculate steering amount
  float steering = Kp * (STEER_RESOLUTION * MAX_LF_STEER);
  if ((prev_steer * delta > 0.0) && (abs(prev_steer) > steering))
    steering = steering - Kd * (abs(prev_steer) - steering);
  if (steering < 0.0)
    steering = 0.0;
  if (delta < 0.0)
    steering = -1.0 * steering;

  // confirm that steering doesnt exceed bounds
  if (steering > (STEER_RESOLUTION * MAX_LF_STEER))
    steering = (STEER_RESOLUTION * MAX_LF_STEER);
  else if (steering < (-1 * STEER_RESOLUTION * MAX_LF_STEER))
    steering = (-1 * STEER_RESOLUTION * MAX_LF_STEER);
  return steering;
}

float lineFollow(int exit_case, float prev_steer)
{
  // read colour sensors and calculat steering amount
  ColourReading col_in = read_colour();
  float steering = followAlgorithm(exit_case, col_in, prev_steer);
  // stop line following if exit case is met
  if (steering == STEER_RESOLUTION + 1)
    return STEER_RESOLUTION + 1;
  // throttle speed during sharp turns
  float speed = DRIVE_SPEED;
  if (abs(steering) > (0.8 * (STEER_RESOLUTION * MAX_LF_STEER)))
    speed = 0.7 * speed;
  // run motors at calculated steering and speed
  drive_motors(forward, steering, speed);
  return steering;
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
  // setup_servo();
  // Serial.println("Set up servo motor");
  setup_motors();
  Serial.println("Set up dc motors");
  setup_colour();
  Serial.println("Set up colour sensors");
  Serial.println("Setup complete");
}

// TODO - set up flow for overall process - currently just line follows and stops at bullseye
void loop()
{
  float prev_steer = 0;
  while (1)
  {
    prev_steer = lineFollow(bullseye, prev_steer);
    if (prev_steer == -1)
      break;
  }
  drive_motors(reverse, 0, 50);
  while (1)
  {
    ColourReading col_in = read_colour();
    if ((col_in.b_l < B_B_L) && (col_in.b_r < B_B_R))
      break;
  }
  drive_motors(forward, 0, 0);

  Serial.println("Shutting down...");
  shutdown_motors();
  Serial.println("Shut down dc motors");
  // shutdown_servo();
  // Serial.println("Shut down servo motor");
  shutdown_colour();
  Serial.println("Shut down colour sensors");
  shutdown_ultrasonic();
  Serial.println("Shut down ultrasonic sensor");
  Serial.println("Shutdown complete");

  while (1)
    ;
}
