#include "Wire.h"
#include "Adafruit_TCS34725.h"
#include "pinout.h"

const int TCSL_SDA = PC9;  // Top pin, All I2C
const int TCSL_SCL = PA8;  // D7
const int TCSR_SDA = PB9;  // D14
const int TCSR_SCL = PB8;  // D15
const int TCS3_SDA = PB3;  // D3
const int TCS3_SCL = PB10; // D6

Adafruit_TCS34725 tcs_left = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);
Adafruit_TCS34725 tcs_right = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);
Adafruit_TCS34725 tcs_three = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);

TwoWire Wire1(TCSL_SDA, TCSL_SCL);
TwoWire Wire2(TCSR_SDA, TCSR_SCL);
TwoWire Wire3(TCS3_SDA, TCS3_SCL);

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
