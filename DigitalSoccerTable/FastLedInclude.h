// **************************************************
// *** FastLED Neopixel
// **************************************************
//#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
//#define FASTLED_ESP8266_D1_PIN_ORDER
#define FASTLED_ESP8266_RAW_PIN_ORDER

//#define FASTLED_INTERRUPT_RETRY_COUNT 0
//#define FASTLED_ESP8266_DMA
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
FASTLED_USING_NAMESPACE

#define PIXEL_PIN D3
#define PIXEL_TYPE NEOPIXEL

#define PIXEL_COUNT 124
#define PIXEL_USE_OFFSET
#define PIXEL_OFFSET_ZERO
#ifdef PIXEL_USE_OFFSET
#ifdef PIXEL_OFFSET_ZERO
#define PIXEL_OFFSET 0
#else
#define PIXEL_OFFSET (PIXEL_COUNT / 4)
#endif
#endif

// Static size:
extern struct CRGB leds[];
// Dynamic size:
//extern struct CRGB *leds;
