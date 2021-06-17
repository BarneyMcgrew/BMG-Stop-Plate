#include <SSD1306AsciiWire.h>
#include <FastLED.h>

// REGION | Global Definitions
  // 0X3C+SA0 - 0x3C or 0x3D
  #define I2C_ADDRESS 0x3C
  #define LED_DATA_PIN 7
  #define NUM_LEDS 1
  #define BRIGHTNESS 255
  #define LED_TYPE WS2812
  #define COLOR_ORDER RGB  
  #define SENSITIVTY_EEPROM_ADDRESS 200
  CRGB leds[NUM_LEDS];

  // Define proper RST_PIN if required.
  #define RST_PIN -1
  SSD1306AsciiWire oled;