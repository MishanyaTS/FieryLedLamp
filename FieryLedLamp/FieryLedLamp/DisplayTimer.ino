#if USE_TM1637

enum TM1637_View : uint8_t {
  TM1637_VIEW_NONE        = 0,
  TM1637_VIEW_CLOCK       = 1,
  TM1637_VIEW_WEATHER     = 2,
  TM1637_VIEW_WEATHER_ERR = 3,
  TM1637_VIEW_DASH        = 4
};

static uint8_t tm1637LastView        = 255;
static uint8_t tm1637LastHour        = 255;
static uint8_t tm1637LastMinute      = 255;
static int16_t tm1637LastTemp        = 1000;
static bool    tm1637LastErrBlink    = false;
static uint8_t tm1637LastDisplayFlag = 255;

static void tm1637InvalidateCache() {
  tm1637LastView        = 255;
  tm1637LastHour        = 255;
  tm1637LastMinute      = 255;
  tm1637LastTemp        = 1000;
  tm1637LastErrBlink    = false;
  tm1637LastDisplayFlag = 255;
}

static void tm1637DrawWeatherValue(int8_t t) {
  display.point(false);

  bool neg = t < 0;
  int8_t abs_t = abs(t);
  if (abs_t > 99) abs_t = 99;

  uint8_t d10 = abs_t / 10;
  uint8_t d1  = abs_t % 10;

  if (neg) {
    if (abs_t == 0) {
      display.displayByte(_empty, display.encodeDigit(0), _deg, _C);
    } else if (abs_t < 10) {
      display.displayByte(_dash, display.encodeDigit(d1), _deg, _C);
    } else {
      display.displayByte(_dash, display.encodeDigit(d10), display.encodeDigit(d1), _deg);
    }
  } else {
    if (abs_t == 0) {
      display.displayByte(_empty, display.encodeDigit(0), _deg, _C);
    } else if (abs_t < 10) {
      display.displayByte(_empty, display.encodeDigit(d1), _deg, _C);
    } else {
      display.displayByte(display.encodeDigit(d10), display.encodeDigit(d1), _deg, _C);
    }
  }
}

void Display_Timer (uint8_t argument)   {
    if (!tm1637_on) return;
    
    if (DisplayFlag ==0 && LastEffect != currentMode) {
        LastEffect = currentMode;
        DisplayTimer = millis();
        DisplayFlag = 1;
        tm1637InvalidateCache();
        uint8_t n = currentMode;
        if (n < 100) {
        display.point(1);
        display.displayByte(_E_, _F_, _empty, _empty);               // отображаем EF
        display.showNumberDec (n, 0, 2, 2);                // отображаем номер эффекта
        }
        else {
        display.point(0);
        display.displayByte(_E_, _empty, _empty, _empty);               // отображаем EF
        display.showNumberDec (n, 0, 3, 1);                // отображаем номер эффекта
        }
    }
    if (DisplayFlag == 1 && (millis() - DisplayTimer > 2000)) {
        DisplayFlag = 0;
        tm1637InvalidateCache();
        if (timeSynched) clockTicker_blink ();                   // отображаем  время
        else display.displayByte(_dash, _dash, _dash, _dash);    // отображаем прочерки
    }
    if(DisplayFlag == 3) {
        DisplayTimer = millis();
        DisplayFlag = 4;
        tm1637InvalidateCache();
        display.point(0);
        display.displayByte(_empty, _empty, _empty, _empty);               // отображаем ничего
        display.showNumberDec (argument, 0, 3, 1);              // отображаем значение argument (br, sc, sp)
    }
    if (DisplayFlag == 4 && (millis() - DisplayTimer > 3000)) {
        DisplayFlag = 0;
        tm1637InvalidateCache();
        if (timeSynched) clockTicker_blink ();                   // отображаем  время
        else display.displayByte(_dash, _dash, _dash, _dash);    // отображаем прочерки
    }  

// Режим "ЧАСЫ / ПОГОДА"
if (DisplayFlag == 0) {
      if (!timeSynched) {
        if (tm1637LastView != TM1637_VIEW_DASH || tm1637LastDisplayFlag != DisplayFlag) {
          display.displayByte(_dash, _dash, _dash, _dash);
          tm1637LastView        = TM1637_VIEW_DASH;
          tm1637LastDisplayFlag = DisplayFlag;
        }
    return;
      }

  #if (USE_WEATHER == 0)
    showClock = true;
    weatherErrActive = false;
  #else
  if (inClockWeatherMode) {
    // ===== ПЕРЕКЛЮЧЕНИЕ =====
    if (showClock) {
      if (millis() - displaySwitchTimer >= CLOCK_SHOW_INTERVAL) {
        displaySwitchTimer = millis();
        showClock = false;
        tm1637InvalidateCache();
      }
    } else {
      if (weatherErrActive) {
        if (millis() - weatherErrTimer >= WEATHER_ERR_TIME) {
          weatherErrActive = false;
          showClock = true;
          displaySwitchTimer = millis();
          tm1637InvalidateCache();
        }
      } else {
        if (millis() - displaySwitchTimer >= WEATHER_SHOW_INTERVAL) {
          displaySwitchTimer = millis();
          showClock = true;
                tm1637InvalidateCache();
              }
            }
          }
        } else {
          showClock = true;
        }
      #endif

    // ===== ОТОБРАЖЕНИЕ =====
      if (!inClockWeatherMode || showClock) {
        bool needDraw = (tm1637LastView != TM1637_VIEW_CLOCK) ||
                        (tm1637LastDisplayFlag != DisplayFlag) ||
                        (tm1637LastHour != hours) ||
                        (tm1637LastMinute != last_minute);

        if (needDraw) {
          clockTicker_blink();
          tm1637LastView        = TM1637_VIEW_CLOCK;
          tm1637LastHour        = hours;
          tm1637LastMinute      = last_minute;
          tm1637LastDisplayFlag = DisplayFlag;
        }
        return;
      }

      #if (USE_WEATHER == 1)
      // ---- ПОГОДА ----
      if (currentTemp > -50) {
        weatherErrActive = false;
        int8_t t = round(currentTemp);
          bool needDraw = (tm1637LastView != TM1637_VIEW_WEATHER) ||
                          (tm1637LastDisplayFlag != DisplayFlag) ||
                          (tm1637LastTemp != t);

          if (needDraw) {
            tm1637DrawWeatherValue(t);
            tm1637LastView        = TM1637_VIEW_WEATHER;
            tm1637LastTemp        = t;
            tm1637LastDisplayFlag = DisplayFlag;
          }
      } else {
        // ---- ОШИБКА ПОГОДЫ ----
  if (!weatherErrActive) {
    weatherErrActive = true;
    weatherErrTimer = millis();
    weatherErrBlinkTimer = millis();
    weatherErrBlinkState = true;
    tm1637InvalidateCache();
  }
  // мигание
  if (millis() - weatherErrBlinkTimer >= WEATHER_ERR_BLINK) {
    weatherErrBlinkTimer = millis();
    weatherErrBlinkState = !weatherErrBlinkState;
  }

          bool needDraw = (tm1637LastView != TM1637_VIEW_WEATHER_ERR) ||
                          (tm1637LastDisplayFlag != DisplayFlag) ||
                          (tm1637LastErrBlink != weatherErrBlinkState);

          if (needDraw) {
  if (weatherErrBlinkState) {
    display.displayByte(_empty, _E_, _r_, _r_);
  } else {
    display.displayByte(_empty, _empty, _empty, _empty);
  }
            tm1637LastView        = TM1637_VIEW_WEATHER_ERR;
            tm1637LastErrBlink    = weatherErrBlinkState;
            tm1637LastDisplayFlag = DisplayFlag;
          }

  if (millis() - weatherErrTimer >= WEATHER_ERR_TIME) {
    weatherErrActive = false;
    showClock = true;
    displaySwitchTimer = millis();
    tm1637InvalidateCache();
  }
      }
      #endif
    }
}
#endif  //USE_TM1637
