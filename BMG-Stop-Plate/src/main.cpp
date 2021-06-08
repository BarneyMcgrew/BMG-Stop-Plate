// REGION | Include
  #include <Wire.h>
  #include <SSD1306Ascii.h>
  #include <SSD1306AsciiWire.h>
  #include <FastLED.h>
  #include <EEPROM.h>

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

  TimerStateTypes TS_Status;

  unsigned long START_TIME;
  bool TIME_RECORDED;

  unsigned long HIT_MEMORY[128];
  uint8_t HIT_MEMORY_CURSOR = 0;
  uint8_t HIT_COUNT = 0;

  int SENSITIVITY_INTERALS = 5;
  int CURRENT_SENSITIVITY = 100;
  int MAX_SENSITIVITY = 200; 

  int BUZZER_TIME = 750;

  bool LED_HIT_STAGE;

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
    if (hitNo.length() == 1) {
      hitNo = "0" + hitNo;
    }

    if (seconds.length() == 1) {
      seconds = "0" + seconds;
    }

    if (centiSecond.length() == 1) {
      centiSecond = "0" + centiSecond;
    }

    oled.clear();    
    oled.setFont(SystemFont5x7);
    oled.set1X();
    oled.print("  SHOT       TIME(s)");
    oled.println();
    oled.println();
    oled.set2X();
    oled.setFont(Arial_bold_14); 
    oled.print(" " + hitNo);

    if (seconds.length() == 3) {
      oled.print(" ");
    } else {
      oled.print("  ");
    }
    
    oled.print(" " + seconds);
    oled.set1X();
    oled.println("." + centiSecond); 
    oled.println();
    oled.println();
    oled.setFont(Iain5x7);
    oled.println("       Press Stop When Done");
  }

  void DisplaySensitivityChanged() {
    String currentValue = String(((MAX_SENSITIVITY+SENSITIVITY_INTERALS) - CURRENT_SENSITIVITY)/SENSITIVITY_INTERALS);
    String maxValue = String(MAX_SENSITIVITY/SENSITIVITY_INTERALS);
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
    if (hitNo.length() == 1) {
      hitNo = "0" + hitNo;
    }

    if (seconds.length() == 1) {
      seconds = "0" + seconds;
    }

    if (centiSecond.length() == 1) {
      centiSecond = "0" + centiSecond;
    }

    oled.clear();    
    oled.setFont(SystemFont5x7);
    oled.set1X();
    oled.print("  SHOT       TIME(s)");
    oled.println();
    oled.println();
    oled.set2X();
    oled.setFont(Arial_bold_14);
    oled.print(" " + hitNo);

    if (seconds.length() == 3) {
      oled.print(" ");
    } else {
      oled.print("   ");
    }
    oled.print(seconds);
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

// REGION | Set Timer State Functions

  void SetTimerIdle() {
    LedOrange();
    TS_Status = TimerStateTypes::TS_IDLE;
  }

  void SetTimerTiming() {
    LedWhite();
    TS_Status = TimerStateTypes::TS_TIMING;
  }

  void SetTimerReview() {
    LedOrange();
    TS_Status = TimerStateTypes::TS_REVIEW;
  }

// REGION | Functions

  ButtonPressed GetButtonPressed() {
    int val;

    val = digitalRead(BTN_Start);
    if (val == LOW) {
      SW_State_Start = true;
    } else if (SW_State_Start) {      
      SW_State_Start = false;
      return ButtonPressed::BTN_START;
    }

    val = digitalRead(BTN_Stop);
    if (val == LOW) {
      SW_State_Stop = true;
    } else if (SW_State_Stop) {      
      SW_State_Stop = false;
      return ButtonPressed::BTN_STOP;
    }

    val = digitalRead(BTN_Forward);
    if (val == LOW) {
      SW_State_Forward = true;
    } else if (SW_State_Forward) {      
      SW_State_Forward = false;
      return ButtonPressed::BTN_FORWARD;
    }

    val = digitalRead(BTN_Back);
    if (val == LOW) {
      SW_State_Back = true;
    } else if (SW_State_Back) {      
      SW_State_Back = false;
      return ButtonPressed::BTN_BACK;
    }

    val = digitalRead(BTN_SenseUp);
    if (val == LOW) {
      SW_State_SenseUp = true;
    } else if (SW_State_SenseUp) {      
      SW_State_SenseUp = false;
      return ButtonPressed::BTN_SENSITIVTY_UP;
    }

    val = digitalRead(BTN_SenseDw);
    if (val == LOW) {
      SW_State_SenseDw = true;
    } else if (SW_State_SenseDw) {      
      SW_State_SenseDw = false;
      return ButtonPressed::BTN_SENSITIVTY_DWN;
    }

    return ButtonPressed::BTN_NONE;
  }

  void ShowStart() {
    DisplayStartTimer();
  }

  void StartTimer() {    
    DisplayTimeRecorded("00", "00", "00");
    START_TIME = millis();
    //SoundBuzzer();
    TIME_RECORDED = false;
  }

  void ShowReview() {
    if ((HIT_COUNT == 0) or (!TIME_RECORDED && TS_Status == TimerStateTypes::TS_TIMING)) {
      SetTimerIdle();
      ShowStart();
      return;
    }
      
    SetTimerReview();

    String hitNo = String(HIT_MEMORY_CURSOR);
    unsigned long reviewTime = HIT_MEMORY[HIT_MEMORY_CURSOR-1];

    String seconds = String(reviewTime / 100);
    String centiSecond = String(reviewTime - ((reviewTime / 100) * 100));
    DisplayTimeReview(hitNo, seconds, centiSecond);
  }

  void MoveReviewForward() {
    if (HIT_MEMORY_CURSOR == HIT_COUNT)
      return;

    HIT_MEMORY_CURSOR += 1;
  }

  void MoveReviewBackward() {
    if (HIT_MEMORY_CURSOR == 1)
      return;

    HIT_MEMORY_CURSOR -= 1;
  }

  void ShowSensitivity() {
    DisplaySensitivityChanged();

    // EEPROM Store
    EEPROM.write(SENSITIVTY_EEPROM_ADDRESS, CURRENT_SENSITIVITY);
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
    if (!TIME_RECORDED) {
      TIME_RECORDED = true;
      HIT_COUNT = 0;
      HIT_MEMORY_CURSOR = 0;
      LED_HIT_STAGE = true;
    } 

    if (LED_HIT_STAGE) {
      LedBlue();
    } else {
      LedGreen();
    }

    LED_HIT_STAGE = !LED_HIT_STAGE;

    unsigned long currentTime = millis();
    unsigned long splitTime = (currentTime - START_TIME) / 10;

    HIT_MEMORY_CURSOR++;
    HIT_COUNT = HIT_MEMORY_CURSOR;
    HIT_MEMORY[HIT_MEMORY_CURSOR-1] = splitTime;

    String hitNo = String(HIT_MEMORY_CURSOR);
    unsigned long reviewTime = HIT_MEMORY[HIT_MEMORY_CURSOR-1];

    String seconds = String(reviewTime / 100);
    String centiSecond = String(reviewTime - ((reviewTime / 100) * 100));

    Serial.println();

    DisplayTimeRecorded(hitNo, seconds, centiSecond);
  }

  void CheckStopPlateHit() {
    int piezoSensor = analogRead(IO_Sensor);
    if (piezoSensor >= CURRENT_SENSITIVITY) {
      RecordStopPlateHit();
    }
  }

// REGION | State Functions
  void IdleLoopProcess(ButtonPressed button){
    switch (button)
    {
      case ButtonPressed::BTN_START:
        SetTimerTiming();
        StartTimer();        
        break;
      case ButtonPressed::BTN_STOP:
        ShowReview();
        SetTimerReview();
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
        SetTimerTiming();
        StartTimer();  
        break;
      case ButtonPressed::BTN_STOP:
        ShowStart();
        SetTimerIdle();
        break;
      case ButtonPressed::BTN_FORWARD:
        MoveReviewForward();
        ShowReview();
        SetTimerReview();
        break;
      case ButtonPressed::BTN_BACK:
        MoveReviewBackward();
        ShowReview();
        SetTimerReview();
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
    // Serial Debug Setup
    Serial.begin(9600);

    // Get EEPROM Values
    int storedSensitivity = EEPROM.read(SENSITIVTY_EEPROM_ADDRESS);
    if (storedSensitivity > 0 && storedSensitivity <= MAX_SENSITIVITY) {
      CURRENT_SENSITIVITY = storedSensitivity;
    }

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
    delay(2000);
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
    
    // Timer State Setup
    SetTimerIdle();
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