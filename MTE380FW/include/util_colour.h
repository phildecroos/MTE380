#include "Wire.h"
#include "Adafruit_TCS34725.h"
#include "config.h"

Adafruit_TCS34725 tcs_left = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);
Adafruit_TCS34725 tcs_right = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);
Adafruit_TCS34725 tcs_three = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);

TwoWire Wire1(PC9, PA8);
TwoWire Wire2(PB9, PB8);
TwoWire Wire3(PB3, PB10);

class ColourReading
{
public:
  int r_l;
  int g_l;
  int b_l;
  int r_r;
  int g_r;
  int b_r;
  int r_3;
  int g_3;
  int b_3;

  ColourReading(int r_l_new, int g_l_new, int b_l_new, int r_r_new, int g_r_new, int b_r_new, int r_3_new, int g_3_new, int b_3_new)
  {
    r_l = r_l_new;
    g_l = g_l_new;
    b_l = b_l_new;
    r_r = r_r_new;
    g_r = g_r_new;
    b_r = b_r_new;
    r_3 = r_3_new;
    g_3 = g_3_new;
    b_3 = b_3_new;
  }
};

void setup_colour()
{
  if (tcs_left.begin(0x29, &Wire1))
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

  if (tcs_three.begin(0x29, &Wire3))
  {
    Serial.println("Found third colour sensor");
  }
  else
  {
    Serial.println("No TCS34725_three found ... check your connections");
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
  uint16_t r_three, g_three, b_three, c_three, colorTemp_three, lux_three;

  tcs_left.getRawData(&r_left, &g_left, &b_left, &c_left);
  tcs_right.getRawData(&r_right, &g_right, &b_right, &c_right);
  tcs_three.getRawData(&r_three, &g_three, &b_three, &c_three);

  return ColourReading(r_left, g_left, b_left, r_right, g_right, b_right, r_three, g_three, b_three);
}
