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

enum red_rgb // apprx readings of colour sensors when fully on red line
{
  r_r = 1400,
  g_r = 400,
  b_r = 300
};

enum green_rgb // apprx readings of colour sensors when fully on green safezone line
{
  r_g = 1625,
  g_g = 1450,
  b_g = 800
};

enum blue_rgb // apprx readings of colour sensors when fully on blue bullseye line
{
  r_b = 1625,
  g_b = 1450,
  b_b = 800
};

// TODO - make benchmark program to read each sensor, do each computation, and set each output as quickly as possible for n seconds & print results
void roboBench()
{
}

// TODO - make demo program to drive around, steer, and read sensors + print values
void demoSupremo()
{
}

// TODO - make calibration route
void calibr8()
{
}

// TODO - make line following control algorithm here
int hollowFollow(ColourReading col_in)
{
  float L_error = abs((col_in.r_l - r_r) + (col_in.g_l - g_r) + (col_in.b_l - b_r));
  float R_error = abs((col_in.r_r - r_r) + (col_in.g_r - g_r) + (col_in.b_r - b_r));

  Serial.println("L error: ");
  Serial.println(L_error);
  Serial.println("R error: ");
  Serial.println(R_error);

  int threshold = 1.3;
  int steering;
  if (R_error > (threshold * L_error))
  {
    steering = 4; //(STEER_MAX - ((L_error / R_error) * STEER_MAX));
  }
  else if (L_error > (threshold * R_error))
  {
    steering = -4; //-1 * (STEER_MAX - ((R_error / L_error) * STEER_MAX));
  }
  else
  {
    steering = 0;
  }

  return steering;
}

// TODO - make line following program
void lineTime()
{
  while (1)
  {
    ColourReading col_in = read_colour();

    Serial.print("\n---------------------\n");

    // Serial.print("R Left: ");
    // Serial.print(col_in.r_l, DEC);
    // Serial.print(" ");
    // Serial.print("G Left: ");
    // Serial.print(col_in.g_l, DEC);
    // Serial.print(" ");
    // Serial.print("B Left: ");
    // Serial.print(col_in.b_l, DEC);
    // Serial.print(" ");
    // Serial.println(" ");

    // Serial.print("R Right: ");
    // Serial.print(col_in.r_r, DEC);
    // Serial.print(" ");
    // Serial.print("G Right: ");
    // Serial.print(col_in.g_r, DEC);
    // Serial.print(" ");
    // Serial.print("B Right: ");
    // Serial.print(col_in.b_r, DEC);
    // Serial.print(" ");
    // Serial.println(" ");

    int steering = hollowFollow(col_in);

    Serial.println("steering: ");
    Serial.println(steering);

    drive_motors(forward, steering, SPEED);
  }
}

// TODO - make pickup routine here
void poopNscoop()
{
}

// TODO - make dropoff routine here
void stopNdrop()
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

// TODO - set up overall control flow for total process
void loop()
{
  lineTime();

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
}
