#include "Wire.h"
#include "Adafruit_TCS34725.h"
#include "config.h"

// TODO - test different integration times and gains
Adafruit_TCS34725 tcs_left = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X);
Adafruit_TCS34725 tcs_right = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X);

TwoWire Wire2(PC9, PA8);

class ColourReading
{
public:
  int r_l;
  int g_l;
  int b_l;
  int r_r;
  int g_r;
  int b_r;

  ColourReading(int r_l_new, int g_l_new, int b_l_new, int r_r_new, int g_r_new, int b_r_new)
  {
    r_l = r_l_new;
    g_l = g_l_new;
    b_l = b_l_new;
    r_r = r_r_new;
    g_r = g_r_new;
    b_r = b_r_new;
  }
};

void setup_colour()
{
  if (tcs_left.begin())
  {
    Serial.println("Found left colour sensor");
  }
  else
  {
    Serial.println("No TCS34725_left found ... check your connections");
    while (1)
      ;
  }

  if (tcs_right.begin(0x29, &Wire2))
  {
    Serial.println("Found right colour sensor");
  }
  else
  {
    Serial.println("No TCS34725_right found ... check your connections");
    while (1)
      ;
  }
}

void shutdown_colour()
{
  // can't think of anything that needs to be shut down here
}

ColourReading read_colour()
{
  uint16_t r_left, g_left, b_left, c_left, colorTemp_left, lux_left;
  uint16_t r_right, g_right, b_right, c_right, colorTemp_right, lux_right;

  tcs_left.getRawData(&r_left, &g_left, &b_left, &c_left);
  tcs_right.getRawData(&r_right, &g_right, &b_right, &c_right);

  return ColourReading(r_left, g_left, b_left, r_right, g_right, b_right);
}
