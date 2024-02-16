#include "config.h"
#include "util_colour.h"
#include "util_servo.h"
#include "util_motors.h"
#include "util_ultrasonic.h"

const int SPEED = MOTOR_SPEED;
const int STEER_MAX = MOTOR_STEER;

// TODO - integrate this with calibration routine
enum wood_rgb
{   r_w = 1000,
    g_w = 1000,
    b_w = 500
};

void setup() {
  Serial.begin(9600);
  Serial.println("Setting up...");

  setup_ultrasonic();
  setup_servo();
  setup_motors();
  setup_colour();

  Serial.println("Setup complete");
}

// TODO - set up overall control flow (only line following is here for now)
void loop() {
  // ultrasonic sensor and servo not added yet
  // int us_dist = read_ultrasonic();
  // Serial.println("Distance: "); Serial.println(us_dist);
  // move_servo(0);

  ColourReading col_in = read_colour();

  int L_error = (col_in.r_l - r_w) + (col_in.g_l - g_w) + (col_in.b_l - b_w);
  int R_error = (col_in.r_r - r_w) + (col_in.g_r - g_w) + (col_in.b_r - b_w);

  int steering;
  if (R_error > (1.1 * L_error)) {
    steering = 0.5 * STEER_MAX;
  }
  else if (L_error > (1.1 * R_error)) {
    steering = 0.5 * STEER_MAX;
  }
  else {
    steering = 0;
  }

  drive_motors(true, steering, SPEED);
}
