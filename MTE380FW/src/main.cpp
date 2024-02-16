#include "config.h"
#include "util_colour.h"
#include "util_servo.h"
#include "util_motors.h"
#include "util_ultrasonic.h"

void setup() {
  Serial.begin(9600);
  Serial.println("Setting up...");

  // setup_ultrasonic();
  // setup_servo();
  // setup_motors();
  setup_colour();

  Serial.println("Setup complete");
}

// TODO - set up main logic and flow
void loop() {
  // int us_dist = read_ultrasonic();
  // Serial.println("Distance: "); Serial.println(us_dist);

  read_colour();

  // drive_motors(1);

  // move_servo(0);

  delay(1000);
}
