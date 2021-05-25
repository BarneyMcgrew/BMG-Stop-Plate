#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include <toneAC.h>
#include <FastLED.h>

/// Global Definitions
// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
#define LED_DATA_PIN 7
#define NUM_LEDS 1
#define BRIGHTNESS 255
#define LED_TYPE WS2812
#define COLOR_ORDER RGB
CRGB leds[NUM_LEDS];

// Define proper RST_PIN if required.
#define RST_PIN -1
SSD1306AsciiWire oled;

/// Global Inputs
const int PROGMEM BTN_Start = 2;
const int PROGMEM BTN_Stop = 3;
const int PROGMEM BTN_Forward = 4;
const int PROGMEM BTN_Back = 5;
const int PROGMEM BTN_SenseUp = 8;
const int PROGMEM BTN_SenseDw = 9;
const byte PROGMEM IO_Buzzer = 6;
const int PROGMEM IO_Sensor = A0;

bool SW_State_Start = false;
bool SW_State_Stop = false;
bool SW_State_Back = false;
bool SW_State_Forward = false;
bool SW_State_SenseUp = false;
bool SW_State_SenseDw = false;

/// Global Variables

#pragma region Display Functions

void DisplaySplashScreen() {
  oled.clear();    
  oled.print(" Barney McGrew Ltd (Alpha v1.0)");
  oled.println();
  oled.println();
  oled.set2X();
  oled.println(" BMG STOP PLATE");
  oled.set1X();
  oled.println();
  oled.println();
  oled.println("         'Slippery like Cheese'");
}

void DisplayDebugMessage(String message) {
  oled.clear();    
  oled.print(" Barney McGrew Ltd (Alpha v1.0)");
  oled.println();
  oled.println();
  oled.set2X();
  oled.println(" BMG STOP PLATE");
  oled.set1X();
  oled.println();
  oled.println();
  oled.println(message);
}

#pragma endregion

#pragma region Input Functions

void StartPressed() {
  int val = digitalRead(BTN_Start);
  if (val == LOW) {
    SW_State_Start = true;
  } else if (SW_State_Start) {
    DisplayDebugMessage("START BUTTON PRESSED");
    SW_State_Start = false;
  }
}

void StopPressed() {
  int val = digitalRead(BTN_Stop);
  if (val == LOW) {
    SW_State_Stop = true;
  } else if (SW_State_Stop) {
    DisplayDebugMessage("STOP/REVIEW BUTTON PRESSED");
    SW_State_Stop = false;
  }
}

void ForwardPressed() {
  int val = digitalRead(BTN_Forward);
  if (val == LOW) {
    SW_State_Forward = true;
  } else if (SW_State_Forward) {
    DisplayDebugMessage("FORWARD BUTTON PRESSED");
    SW_State_Forward = false;
  }
}

void BackPressed() {
  int val = digitalRead(BTN_Back);
  if (val == LOW) {
    SW_State_Back = true;
  } else if (SW_State_Back) {
    DisplayDebugMessage("BACK BUTTON PRESSED");
    SW_State_Back = false;
  }
}

void SenseUpPressed() {
  int val = digitalRead(BTN_SenseUp);
  if (val == LOW) {
    SW_State_SenseUp = true;
  } else if (SW_State_SenseUp) {
    DisplayDebugMessage("SENSEUP BUTTON PRESSED");
    SW_State_SenseUp = false;
  }
}

void SenseDwPressed() {
  int val = digitalRead(BTN_SenseDw);
  if (val == LOW) {
    SW_State_SenseDw = true;
  } else if (SW_State_SenseDw) {
    DisplayDebugMessage("SENSEDW BUTTON PRESSED");
    SW_State_SenseDw = false;
  }
}

#pragma endregion

/// Global Setup
void setup() {
  // Wire setup  
  Wire.begin();
  Wire.setClock(400000L);

  #if RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
  #else 
    oled.begin(&Adafruit128x64, I2C_ADDRESS);
  #endif 

  // Display Setup
  oled.setFont(Iain5x7);

  DisplaySplashScreen();

  // IO Setup
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
}

// Global Loop Cycle
void loop() {
  StartPressed();
  StopPressed();
  ForwardPressed();
  BackPressed();
  SenseUpPressed();
  SenseDwPressed();
}

