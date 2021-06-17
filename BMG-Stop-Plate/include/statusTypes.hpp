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
