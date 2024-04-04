#include "Wire.h"
#include "Adafruit_TCS34725.h"
#include "pinout.h"

int tcsl_sda = TCSL_SDA;
int tcsl_scl = TCSL_SCL;
int tcsr_sda = TCSR_SDA;
int tcsr_scl = TCSR_SCL;
int tcs3_sda = TCS3_SDA;
int tcs3_scl = TCS3_SCL;

Adafruit_TCS34725 tcs_left = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);
Adafruit_TCS34725 tcs_right = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);
Adafruit_TCS34725 tcs_three = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);

TwoWire Wire1(tcsl_sda, tcsl_scl);
TwoWire Wire2(tcsr_sda, tcsr_scl);
TwoWire Wire3(tcs3_sda, tcs3_scl);

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

bool setup_colour()
{
  return ((tcs_left.begin(0x29, &Wire1)) && (tcs_right.begin(0x29, &Wire2)) && (tcs_three.begin(0x29, &Wire3)));
}

ColourReading read_colour()
{
  uint16_t r_left, g_left, b_left, c_left, r_right, g_right, b_right, c_right, r_three, g_three, b_three, c_three;

  tcs_left.getRawData(&r_left, &g_left, &b_left, &c_left);
  tcs_right.getRawData(&r_right, &g_right, &b_right, &c_right);
  tcs_three.getRawData(&r_three, &g_three, &b_three, &c_three);

  return ColourReading(r_left, g_left, b_left, r_right, g_right, b_right, r_three, g_three, b_three);
}
