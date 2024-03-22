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

const float G_R_R = 90.0;  // calibrated green readings on tape and wood
const float G_W_R = 500.0;
const float B_B_R = 190.0;
const float G_R_L = 90.0;
const float G_W_L = 500.0;
const float B_B_L = 190.0;
const float WEIGHTS[2] = {1.0, 1.0};

void demoDrive()
{
  drive_motors(forward, 0, SPEED);
  // delay(3000);
  // drive_motors(reverse, 0, SPEED);
  // delay(3000);
  // drive_motors(forward, 0, 0);
  // drive_motors(forward, 40, SPEED);
  // delay(2000);
  // drive_motors(reverse, -40, SPEED);
  // delay(2000);
  // drive_motors(inplace, 100, SPEED);
  // delay(2000);
  // drive_motors(inplace, -100, SPEED);
  // delay(2000);
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

void demoPickDrop()
{
  Serial.println("UP");
  move_servo(UP);
  delay(500);

  drive_motors(forward, 0, SPEED);
  delay(1000);

  drive_motors(reverse, 0, 0);
  Serial.println("DOWN");
  move_servo(DOWN);
  delay(500);

  drive_motors(reverse, 0, SPEED);
  delay(1000);

  drive_motors(reverse, 0, 0);
  delay(1000);

  drive_motors(forward, 0, SPEED);
  delay(1000);

  drive_motors(reverse, 0, 0);
  Serial.println("UP");
  move_servo(UP);
  delay(500);

  drive_motors(reverse, 0, SPEED);
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

float followAlgorithm(ColourReading col_in, float prev_steer)
{
  // return steer = -1 if on blue
  if ((col_in.b_l > B_B_L) && (col_in.b_r > B_B_R))
    return -1;

  float Kp = 0.5;
  float Kd = 3.0;

  // calculate error
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

  // reduce damping if the error is large enough
  Serial.println(delta_ratio);
  if (delta_ratio > 0.30)
  {
    Kp = 3 * pow(delta_ratio, 1);
    // Kd = 0.0;
  }
  // decrease steering amount in response to error
  float steering = 0;
  if (delta > 0.0)
    steering = Kp * (STEER_MAX * MAX_LF_STEER) * delta_ratio;
  else if (delta < 0.0)
    steering = -1.0 * Kp * (STEER_MAX * MAX_LF_STEER) * delta_ratio;

  // accelerate steering returning to zero
  if ((prev_steer * steering > 0.0) && (abs(prev_steer) > abs(steering)))
  {
    if (steering > 0)
      steering = steering - Kd * (abs(prev_steer) - abs(steering));
      if (steering < 0)
        steering = 0;
    else if (steering < 0)
      steering = steering + Kd * (abs(prev_steer) - abs(steering));
      if (steering > 0)
        steering = 0;
  }

  if (steering > (STEER_MAX * MAX_LF_STEER))
    steering = (STEER_MAX * MAX_LF_STEER);
  else if (steering < (-1 * STEER_MAX * MAX_LF_STEER))
    steering = (-1 * STEER_MAX * MAX_LF_STEER);
  return steering;
}

float lineFollow(float prev_steer)
{
  ColourReading col_in = read_colour();
  float steering = followAlgorithm(col_in, prev_steer);
  if (steering == -1)
    return -1;
  float speed = SPEED;
  if (abs(steering) > (0.8 * (STEER_MAX * MAX_LF_STEER)))
    speed = 0.7 * SPEED;
  // Serial.println("Steering: ");
  // Serial.println(steering);
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

// TODO - set up flow for overall process
void loop()
{
  float prev_steer = 0;
  while (1)
  {
    // printCalibrationData();
    prev_steer = lineFollow(prev_steer);
    if (prev_steer == -1)
      break;
  }
  // for (int i = SPEED; i >= 0; i--)
  // {
  //   drive_motors(forward, 0, i);
  //   delay(10);
  // }
  // for (int i = 0; i <= SPEED; i++)
  // {
  //   drive_motors(reverse, 0, i);
  //   delay(10);
  // }
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
