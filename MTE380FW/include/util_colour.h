#include "Wire.h"
#include "Adafruit_TCS34725.h"
#include "config.h"

Adafruit_TCS34725 tcs_left = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
Adafruit_TCS34725 tcs_right = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);

TwoWire Wire2(PC9, PA8);

void setup_colour() {
  if (tcs_left.begin()) {
    Serial.println("Found leftsensor");
  }
  else {
    Serial.println("No TCS34725_left found ... check your connections");
    while (1);
  }

  if (tcs_right.begin(0x29, &Wire2)) {
    Serial.println("Found right sensor");
  }
  else {
    Serial.println("No TCS34725_right found ... check your connections");
    while (1);
  }
}

// TODO: create colour_reading class to return the reading values here instead of just printing

void read_colour() {
  uint16_t r_left, g_left, b_left, c_left, colorTemp_left, lux_left;
  uint16_t r_right, g_right, b_right, c_right, colorTemp_right, lux_right;

  tcs_left.getRawData(&r_left, &g_left, &b_left, &c_left);
  tcs_right.getRawData(&r_right, &g_right, &b_right, &c_right);

  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  colorTemp_left = tcs_left.calculateColorTemperature_dn40(r_left, g_left, b_left, c_left);
  lux_left = tcs_left.calculateLux(r_left, g_left, b_left);
  colorTemp_right = tcs_right.calculateColorTemperature_dn40(r_right, g_right, b_right, c_right);
  lux_right = tcs_right.calculateLux(r_right, g_right, b_right);

  Serial.print("Color Temp Left: "); Serial.print(colorTemp_left, DEC); Serial.print(" K - ");
  Serial.print("Lux Left: "); Serial.print(lux_left, DEC); Serial.print(" - ");
  Serial.print("R Left: "); Serial.print(r_left, DEC); Serial.print(" ");
  Serial.print("G Left: "); Serial.print(g_left, DEC); Serial.print(" ");
  Serial.print("B Left: "); Serial.print(b_left, DEC); Serial.print(" ");
  Serial.print("C Left: "); Serial.print(c_left, DEC); Serial.print(" ");
  Serial.println(" ");

  Serial.print("Color Temp Right: "); Serial.print(colorTemp_right, DEC); Serial.print(" K - ");
  Serial.print("Lux Right: "); Serial.print(lux_right, DEC); Serial.print(" - ");
  Serial.print("R Right: "); Serial.print(r_right, DEC); Serial.print(" ");
  Serial.print("G Right: "); Serial.print(g_right, DEC); Serial.print(" ");
  Serial.print("B Right: "); Serial.print(b_right, DEC); Serial.print(" ");
  Serial.print("C Right: "); Serial.print(c_right, DEC); Serial.print(" ");
  Serial.println(" ");
}