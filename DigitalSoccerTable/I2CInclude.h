// **************************************************
// *** I2C, OLED, Expander
// **************************************************
#define i2cOLED

#include "Wire.h"
#ifdef i2cOLED
#include "SSD1306Wire.h"
#include "fonts/Dialog_plain_12.h"
#include "fonts/Nimbus_Sans_L_Regular_Condensed_32.h"
#endif
#include "PCF8574.h"

// Pin definitions for I2C OLED
const uint8_t I2C_EXPANDER_ADDRESS = 0x20;
const int I2C_SDA_PIN = D1;
const int I2C_SDC_PIN = D2;
const int I2C_INT_PIN = D4;
const int I2C_BUS_SPEED = 100000; // 100kHz for PCF8574
#ifdef i2cOLED
const uint8_t I2C_DISPLAY_ADDRESS = 0x3c;
#endif

// Digital GPIO expander over I2C
PCF8574 expander;

#ifdef i2cOLED
// 128x64 pixel OLED display over I2C
SSD1306Wire display(I2C_DISPLAY_ADDRESS, I2C_SDA_PIN, I2C_SDC_PIN); // I2C OLED
//OLEDDisplayUi ui(&display);
#endif
