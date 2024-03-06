#include "config.h"
#include "util_colour.h"
#include "util_servo.h"
#include "util_motors.h"
#include "util_ultrasonic.h"

const int SPEED = MOTOR_SPEED;
const int STEER_MAX = MOTOR_STEER;
const int THRESHOLD = COLOUR_THRESHOLD;

// TODO - integrate this with calibration routine
enum wood_rgb
{
  r_w = 1625,
  g_w = 1450,
  b_w = 800
};

void setup()
{
  Serial.begin(9600);
  Serial.println("Setting up...");

  // setup_ultrasonic();
  // Serial.println("Set up ultrasonic sensor");
  // setup_servo();
  // Serial.println("Set up servo motor");
  setup_motors();
  Serial.println("Set up dc motors");
  setup_colour();
  Serial.println("Set up colour sensors");

  Serial.println("Setup complete");
}

// TODO - set up overall control flow (only line following is here for now)
void loop()
{
  // // test using ultrasonic
  // int us_dist = read_ultrasonic();
  // Serial.println("US Distance: "); Serial.println(us_dist);

  // // test using servo
  // move_servo(0);

  // test using colour sensor
  ColourReading col_in = read_colour();

  int L_error = abs((col_in.r_l - r_w) + (col_in.g_l - g_w) + (col_in.b_l - b_w));
  int R_error = abs((col_in.r_r - r_w) + (col_in.g_r - g_w) + (col_in.b_r - b_w));

  Serial.println("L error: ");
  Serial.println(L_error);
  Serial.println("R error: ");
  Serial.println(R_error);

  int steering;
  if (R_error > (THRESHOLD * L_error))
  {
    steering = 0.5 * STEER_MAX;
  }
  else if (L_error > (THRESHOLD * R_error))
  {
    steering = -0.5 * STEER_MAX;
  }
  else
  {
    steering = 0;
  }

  Serial.println("steering: ");
  Serial.println(steering);

  drive_motors(true, steering, SPEED);
}
