#define BUTTON_TEAM_1_PIN D5
#define BUTTON_TEAM_2_PIN D6
#define BUTTON_MAIN_PIN D7
#define BUTTON_SETTINGS_PIN D8

#define i2cOLED
#include "SSD1306Wire.h"

#include "PCF8574.h"

#include "fonts/Dialog_plain_12.h"
#include "fonts/Nimbus_Sans_L_Regular_Condensed_32.h"

// Pin definitions for I2C OLED
const uint8_t I2C_DISPLAY_ADDRESS = 0x3c;
const uint8_t I2C_EXPANDER_ADDRESS = 0x20;
const int I2C_SDA_PIN = D2;
const int I2C_SDC_PIN = D1;
const int I2C_INT_PIN = D5;
const int I2C_BUS_SPEED = 100000; // 100kHz for PCF8574

// 128x64 pixel OLED display over I2C
SSD1306Wire display(I2C_DISPLAY_ADDRESS, I2C_SDA_PIN, I2C_SDC_PIN); // I2C OLED
//OLEDDisplayUi ui(&display);

// Digital GPIO expander over I2C
PCF8574 expander;
