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

const float G_R_R = 170.0; // apprx readings of R/L colour sensors on red tape/wood
const float G_W_R = 930.0;
const float G_R_L = 170.0;
const float G_W_L = 950.0;
const float WEIGHTS[2] = {98.41, 101.59};

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

float followAlgorithm(ColourReading col_in, float prev_steer)
{
  float L_notred = WEIGHTS[0] * abs(col_in.g_l - G_R_L);
  float R_notred = WEIGHTS[1] * abs(col_in.g_r - G_R_R);
  float delta = L_notred - R_notred;
  float delta_ratio;
  if (delta > 0)
  {
    delta_ratio = (delta / (WEIGHTS[0] * abs(G_W_L - G_R_L)));
  }
  else if (delta < 0)
  {
    delta_ratio = (delta / (WEIGHTS[1] * abs(G_W_R - G_R_R)));
  }

  float Kp = 1.0;
  float steering = Kp * (STEER_MAX * MAX_LF_STEER) * delta_ratio;
  Serial.println("P Control Steering: ");
  Serial.println(steering);

  // float Kd = 0.1;
  // if ((prev_steer * steering > 0.0) && (abs(prev_steer) > abs(steering)))
  // {
  //   if (steering > 0)
  //     steering = steering - Kd * (abs(prev_steer) - abs(steering));
  //   else if (steering < 0)
  //     steering = steering + Kd * (abs(prev_steer) - abs(steering));
  // }

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
  Serial.println("Steering: ");
  Serial.println(steering);
  drive_motors(forward, steering, SPEED);
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
  float prev_steer = 0;
  while (1)
  {
  prev_steer = lineFollow(prev_steer);
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
