#include <statusTypes.hpp>

// REGION | Global Variables

  TimerStateTypes TS_Status;

  unsigned long START_TIME;
  bool TIME_RECORDED;

  unsigned long HIT_MEMORY[128];
  uint8_t HIT_MEMORY_CURSOR = 0;
  uint8_t HIT_COUNT = 0;

  int OFFSET_SENSITIVITY = 60;
  int SENSITIVITY_INTERALS = 10;
  int CURRENT_SENSITIVITY = 200;
  int MAX_SENSITIVITY = 400; 

  int BUZZER_TIME = 750;

  bool LED_HIT_STAGE;

  unsigned long LED_White_End; 
  int LED_White_Count;
  bool LED_White_On = false;