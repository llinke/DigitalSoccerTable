#define BUTTON_TEAM_1_PIN D5
#define BUTTON_TEAM_2_PIN D6
#define BUTTON_MAIN_PIN D7
#define BUTTON_SETTINGS_PIN D8

// >>> Uncomment one of the following 2 lines to define which OLED display interface type you are using
//#define spiOLED
#define i2cOLED
#include "SSD1306Wire.h"
//#include "OLEDDisplayUi.h"

#include "fonts/Dialog_plain_12.h"
#include "fonts/Nimbus_Sans_L_Regular_Condensed_32.h"

// Pin definitions for I2C OLED
const int I2C_DISPLAY_ADDRESS = 0x3c;
const int SDA_PIN = D2;
const int SDC_PIN = D1;

SSD1306Wire display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN); // I2C OLED
//OLEDDisplayUi ui(&display);
