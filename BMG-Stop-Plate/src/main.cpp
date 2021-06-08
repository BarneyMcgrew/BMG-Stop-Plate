// REGION | Include
  #include <Wire.h>
  #include <SSD1306Ascii.h>
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
  CRGB leds[NUM_LEDS];

  // Define proper RST_PIN if required.
  #define RST_PIN -1
  SSD1306AsciiWire oled;

// REGION | Global Status Types

  enum class TimerStateTypes: uint8_t
  {
    TS_IDLE = 0,
    TS_TIMING = 1,
    TS_REVIEW = 2
  };

  enum class ButtonPressed: uint8_t
  {
    BTN_START = 2,
    BTN_STOP = 3,
    BTN_FORWARD = 4,
    BTN_BACK = 5,
    BTN_SENSITIVTY_UP = 8,
    BTN_SENSITIVTY_DWN = 9,
    BTN_NONE = 0
  };


// REGION | Global Inputs

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

// REGION | Global Variables

  TimerStateTypes TS_Status = TimerStateTypes::TS_IDLE;

  unsigned long START_TIME;
  bool TIME_RECORDED;

  float HIT_MEMORY[128];
  uint8_t HIT_MEMORY_CURSOR = 0;
  uint8_t HIT_COUNT = 0;

  long LED_Idle_Colour = CRGB::Orange;
  long LED_Timing_Colour = CRGB::White;
  long LED_Hit_Colour_Primary = CRGB::Blue;
  long LED_Hit_Colour_Secondary = CRGB::Green;

  int SENSITIVITY_INTERALS = 10;
  int CURRENT_SENSITIVITY = 50;
  int MAX_SENSITIVITY = 100; 

  int BUZZER_TIME = 750;

  unsigned long LED_White_End; 
  int LED_White_Count;
  bool LED_White_On = false;

// REGION | Buzzer Functions

  void SoundBuzzer() {
    digitalWrite(IO_Buzzer, HIGH);   
    delay(BUZZER_TIME); 
    digitalWrite(IO_Buzzer, LOW);    
  }

// REGION | LED Functions

  void LedGreen() {  
    leds[0] = CRGB::Green;
    FastLED.show();
  }

  void LedBlue() {
    leds[0] = CRGB::Blue;
    FastLED.show();
  }

  void LedWhite() {
    leds[0] = CRGB::White;
    FastLED.show();
  }

  void LedWhiteBlink(int interval, int count) {
    LED_White_End = millis() + interval;
    LED_White_Count = count;
  }

  void LedWhiteBlinkProcess()
  {
    if (LED_White_End > millis())
    {
      leds[0] = CRGB::White;
      FastLED.show();
    } else {
      leds[0] = LED_Idle_Colour;
      FastLED.show();
    }
  }

  void LedRed() {
    leds[0] = CRGB::Red;
    FastLED.show();
  }

  void LedOrange() {
    leds[0] = CRGB::Orange;
    FastLED.show();
  }

// REGION | Display Functions

  void DisplaySplashScreen() {
    oled.clear();    
    oled.setFont(SystemFont5x7);
    oled.set1X();
    oled.print("Barney McGrew (v1.0)");
    oled.println();
    oled.println();
    oled.set2X();
    oled.println("STOP PLATE");
    oled.set1X();
    oled.println();
    oled.println();
    oled.setFont(Wendy3x5);
    oled.println("  'Are You Ready? Standby!'");
  }

  void DisplayStartTimer() {
    oled.clear();    
    oled.setFont(SystemFont5x7);
    oled.set1X();
    oled.print("   PRESS START WHEN");
    oled.println();
    oled.println();
    oled.set2X();
    oled.setFont(Arial_bold_14);
    oled.println("  READY");
    oled.set1X();
    oled.println();
    oled.println();
    oled.setFont(Iain5x7);
    oled.println("          Press Stop To Review");
  }

  void DisplayTimeRecorded(String hitNo, String seconds, String centiSecond) {
    oled.clear();    
    oled.setFont(SystemFont5x7);
    oled.set1X();
    oled.print("  SHOT       TIME(s)");
    oled.println();
    oled.println();
    oled.set2X();
    oled.setFont(Arial_bold_14);
    oled.print(" " + hitNo + "  |");
    oled.print(" " + seconds);
    oled.set1X();
    oled.println("." + centiSecond); 
    oled.println();
    oled.println();
    oled.setFont(Iain5x7);
    oled.println("       Press Stop When Done");
  }

  void DisplaySensitivityChanged() {
    String currentValue = String(MAX_SENSITIVITY - CURRENT_SENSITIVITY);
    String maxValue = String(MAX_SENSITIVITY);
    oled.clear();    
    oled.setFont(SystemFont5x7);
    oled.set1X();
    oled.print("  TIMER SENSITIVITY");
    oled.println();
    oled.println();
    oled.set2X();
    oled.setFont(Arial_bold_14);
    oled.println("   " + currentValue + "/" + maxValue); 
    oled.println();
    oled.println();
    oled.set1X();
    oled.setFont(Iain5x7);
    oled.println("       Press Start When Ready");
  }

  void DisplayTimeReview(String hitNo, String seconds, String centiSecond) {
    oled.clear();    
    oled.setFont(SystemFont5x7);
    oled.set1X();
    oled.print("  SHOT       TIME(s)");
    oled.println();
    oled.println();
    oled.set2X();
    oled.setFont(Arial_bold_14);
    oled.print(" " + hitNo + "  |");
    oled.print(" " + seconds);
    oled.set1X();
    oled.println("." + centiSecond);  
    oled.println();
    oled.println();
    oled.setFont(Iain5x7);
    oled.println("       Press Start When Ready");
  }

  void DisplayDebugMessage(String message) {
    oled.clear();    
    oled.setFont(SystemFont5x7);
    oled.set1X();
    oled.print("Barney McGrew (v1.0)");
    oled.println();
    oled.println();
    oled.set2X();
    oled.println("STOP PLATE");
    oled.set1X();
    oled.println();
    oled.println();
    oled.println(message);
  }


// REGION | Input Functions

  void StartPressed() {
    int val = digitalRead(BTN_Start);
    if (val == LOW) {
      SW_State_Start = true;
    } else if (SW_State_Start) {
      DisplayTimeRecorded("01", "003", "76");
      SoundBuzzer();
      LedWhiteBlink(1000, 5);
      SW_State_Start = false;
    }
  }

  void StopPressed() {
    int val = digitalRead(BTN_Stop);
    if (val == LOW) {
      SW_State_Stop = true;
    } else if (SW_State_Stop) {
      DisplayStartTimer();
      LedOrange();
      SW_State_Stop = false;
    }
  }

  void ForwardPressed() {
    int val = digitalRead(BTN_Forward);
    if (val == LOW) {
      SW_State_Forward = true;
    } else if (SW_State_Forward) {
      DisplayTimeReview("03", "012", "13");
      SW_State_Forward = false;
    }
  }

  void BackPressed() {
    int val = digitalRead(BTN_Back);
    if (val == LOW) {
      SW_State_Back = true;
    } else if (SW_State_Back) {
      DisplayDebugMessage("BACK PRESSED");
      SW_State_Back = false;
    }
  }

  void SenseUpPressed() {
    int val = digitalRead(BTN_SenseUp);
    if (val == LOW) {
      SW_State_SenseUp = true;
    } else if (SW_State_SenseUp) {
      DisplaySensitivityChanged();
      SW_State_SenseUp = false;
    }
  }

  void SenseDwPressed() {
    int val = digitalRead(BTN_SenseDw);
    if (val == LOW) {
      SW_State_SenseDw = true;
    } else if (SW_State_SenseDw) {
      DisplayDebugMessage("SENSEDW PRESSED");
      SW_State_SenseDw = false;
    }
  }

// REGION | Functions

  ButtonPressed GetButtonPressed() {

    int val = digitalRead(BTN_Start);
    if (val == LOW) {
      SW_State_Start = true;
    } else if (SW_State_Start) {      
      SW_State_Start = false;
      return ButtonPressed::BTN_START;
    }

    int val = digitalRead(BTN_Stop);
    if (val == LOW) {
      SW_State_Start = true;
    } else if (SW_State_Start) {      
      SW_State_Start = false;
      return ButtonPressed::BTN_STOP;
    }

    int val = digitalRead(BTN_Forward);
    if (val == LOW) {
      SW_State_Start = true;
    } else if (SW_State_Start) {      
      SW_State_Start = false;
      return ButtonPressed::BTN_FORWARD;
    }

    int val = digitalRead(BTN_Back);
    if (val == LOW) {
      SW_State_Start = true;
    } else if (SW_State_Start) {      
      SW_State_Start = false;
      return ButtonPressed::BTN_BACK;
    }

    int val = digitalRead(BTN_SenseUp);
    if (val == LOW) {
      SW_State_Start = true;
    } else if (SW_State_Start) {      
      SW_State_Start = false;
      return ButtonPressed::BTN_SENSITIVTY_UP;
    }

    int val = digitalRead(BTN_SenseDw);
    if (val == LOW) {
      SW_State_Start = true;
    } else if (SW_State_Start) {      
      SW_State_Start = false;
      return ButtonPressed::BTN_SENSITIVTY_DWN;
    }

    return ButtonPressed::BTN_NONE;
  }

  void ShowStart() {
    DisplayStartTimer();
  }

  void StartTimer() {
    START_TIME = millis();
    SoundBuzzer();
    TIME_RECORDED = false;
    DisplayTimeRecorded("0", "000", "00");
  }

  void ShowReview() {
    if (!TIME_RECORDED)
      return;

    String hitNo = String(HIT_MEMORY_CURSOR);
    float reviewTime = HIT_MEMORY[HIT_MEMORY_CURSOR];
    String seconds = String(reviewTime / 1000, 0);
    String centiSecond = String((reviewTime / 10) - ((reviewTime / 1000) * 100), 0);
    DisplayTimeReview(hitNo, seconds, centiSecond);
  }

  void MoveReviewForward() {
    if (HIT_MEMORY_CURSOR == HIT_COUNT)
      return;

    HIT_MEMORY_CURSOR += 1;
  }

  void MoveReviewBackward() {
    if (HIT_MEMORY_CURSOR == 0)
      return;

    HIT_MEMORY_CURSOR -= 1;
  }

  void ShowSensitivity() {
    DisplaySensitivityChanged();
  }

  void SetSensitivityUp() {
    if (CURRENT_SENSITIVITY == SENSITIVITY_INTERALS)
      return;

    CURRENT_SENSITIVITY -= SENSITIVITY_INTERALS;
  }

  void SetSensitivityDwn() {
    if (CURRENT_SENSITIVITY == MAX_SENSITIVITY)
      return;

    CURRENT_SENSITIVITY += SENSITIVITY_INTERALS;
  }

  void RecordStopPlateHit() {
    //record stop plate hit
  }

  void CheckStopPlateHit() {
    int piezoSensor = digitalRead(IO_Sensor);

    if (piezoSensor >= CURRENT_SENSITIVITY) {
      RecordStopPlateHit();
    }
  }

// REGION | State Functions
  void IdleLoopProcess(ButtonPressed button){
    switch (button)
    {
      case ButtonPressed::BTN_START:
        StartTimer();
        TS_Status = TimerStateTypes::TS_TIMING;
        break;
      case ButtonPressed::BTN_STOP:
        ShowReview();
        TS_Status = TimerStateTypes::TS_REVIEW;
        break;
      case ButtonPressed::BTN_FORWARD:
        // Do Nothing
        break;
      case ButtonPressed::BTN_BACK:
        // Do Nothing
        break;
      case ButtonPressed::BTN_SENSITIVTY_UP:
        SetSensitivityUp();
        ShowSensitivity();
        break;
      case ButtonPressed::BTN_SENSITIVTY_DWN:
        SetSensitivityDwn();
        ShowSensitivity();
        break;
      default:
        // Do Nothing
        break;
    }
  }

  void TimingLoopProcess(ButtonPressed button) {
    switch (button)
    {
      case ButtonPressed::BTN_START:
        // Do Nothing
        break;
      case ButtonPressed::BTN_STOP:
        ShowReview();
        TS_Status = TimerStateTypes::TS_REVIEW;
        break;
      case ButtonPressed::BTN_FORWARD:
        // Do Nothing
        break;
      case ButtonPressed::BTN_BACK:
        // Do Nothing
        break;
      case ButtonPressed::BTN_SENSITIVTY_UP:
        // Do Nothing
        break;
      case ButtonPressed::BTN_SENSITIVTY_DWN:
        // Do Nothing
        break;
      default:
        CheckStopPlateHit();
        break;
    }
  }

  void ReviewLoopProcess(ButtonPressed button) {

    switch (button)
    {
      case ButtonPressed::BTN_START:
        StartTimer();
        TS_Status = TimerStateTypes::TS_TIMING;
        break;
      case ButtonPressed::BTN_STOP:
        ShowStart();
        TS_Status = TimerStateTypes::TS_IDLE;
        break;
      case ButtonPressed::BTN_FORWARD:        
        ShowReview();
        MoveReviewForward();
        TS_Status = TimerStateTypes::TS_REVIEW;
        break;
      case ButtonPressed::BTN_BACK:
        ShowReview();
        MoveReviewBackward();
        TS_Status = TimerStateTypes::TS_REVIEW;
        break;
        break;
      case ButtonPressed::BTN_SENSITIVTY_UP:
        SetSensitivityUp();
        ShowSensitivity();
        break;
      case ButtonPressed::BTN_SENSITIVTY_DWN:
        SetSensitivityDwn();
        ShowSensitivity();
        break;
      default:
        // Do Nothing
        break;
    }
  }

// REGION | Setup

  void setup() {
    // Wire setup  
    Wire.begin();
    Wire.setClock(400000L);

    #if RST_PIN >= 0
      oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
    #else 
      oled.begin(&Adafruit128x64, I2C_ADDRESS);
    #endif 

    // Initial Display Setup
    oled.setFont(SystemFont5x7);

    DisplaySplashScreen();
    delay(1500);
    ShowStart();

    // IO Setup
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);
    pinMode(9, INPUT_PULLUP);
    pinMode(6, OUTPUT);

    // LED Setup Test
    FastLED.addLeds<WS2811, LED_DATA_PIN, RGB>(leds, NUM_LEDS);
    FastLED.setBrightness(  BRIGHTNESS );
    LedOrange();

    // Timer State Setup
    TS_Status = TimerStateTypes::TS_IDLE;
  }

// REGION | Loop

  void loop() {
    // check if button pressed
    ButtonPressed button = GetButtonPressed();

    // check state of timer
    switch (TS_Status)
    {
      case TimerStateTypes::TS_IDLE:
        IdleLoopProcess(button);
        break;
      case TimerStateTypes::TS_TIMING:
        TimingLoopProcess(button);
        break;
      case TimerStateTypes::TS_REVIEW:
        ReviewLoopProcess(button);
        break;
      default:
        break;
    }
  }