// **************************************************
// *** Game Control
// **************************************************
#define BUTTON_TEAM_1_PIN D5
#define BUTTON_TEAM_2_PIN D6
#define BUTTON_MAIN_PIN D7
#define BUTTON_SETTINGS_PIN D8

// **************************************************
// *** I2C, OLED, Expander
// **************************************************
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

// **************************************************
// *** FastLED Neopixel
// **************************************************
/*
//#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
//#define FASTLED_ESP8266_D1_PIN_ORDER
#define FASTLED_ESP8266_RAW_PIN_ORDER

//#define FASTLED_INTERRUPT_RETRY_COUNT 0
//#define FASTLED_ESP8266_DMA
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
FASTLED_USING_NAMESPACE

#define PIXEL_PIN D4

#define PIXEL_COUNT 60
#define PIXEL_OFFSET 15

#define PIXEL_TYPE NEOPIXEL

// Static size:
//extern struct CRGB leds[];
// Dynamic size:
extern struct CRGB *leds;
*/