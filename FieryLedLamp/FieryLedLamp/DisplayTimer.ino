#if USE_TM1637
void Display_Timer (uint8_t argument)   {
    
    if (DisplayFlag ==0 && LastEffect != currentMode) {
        LastEffect = currentMode;
        DisplayTimer = millis();
        DisplayFlag = 1;
        uint8_t n;
        for (n = 0; n < MODE_AMOUNT; n++)
        {
            if (eff_num_correct[n] == currentMode) break;
        }
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
        if (timeSynched) clockTicker_blink ();                   // отображаем  время
        else display.displayByte(_dash, _dash, _dash, _dash);    // отображаем прочерки
    }
/*        
    #if USE_MP3_PLAYER
    if (DisplayFlag ==0 && LastCurrentFolder != CurrentFolder) {
        LastCurrentFolder = CurrentFolder;
        DisplayTimer = millis();
        DisplayFlag = 2;
        display.point(1);
        display.displayByte(_F_, _o_, _empty, _empty);               // отображаем Fo
        display.showNumberDec (CurrentFolder, 0, 2, 2);              // отображаем номер папки
    }
    if (DisplayFlag == 2 && (millis() - DisplayTimer > 3000)) {
        DisplayFlag = 0;
        if (timeSynched) clockTicker_blink ();                   // отображаем  время
        else display.displayByte(_dash, _dash, _dash, _dash);    // отображаем прочерки
    }
    #endif  // USE_MP3_PLAYER
*/    
    if(DisplayFlag == 3) {
        DisplayTimer = millis();
        DisplayFlag = 4;
        display.point(0);
        display.displayByte(_empty, _empty, _empty, _empty);               // отображаем ничего
        display.showNumberDec (argument, 0, 3, 1);              // отображаем значение argument (br, sc, sp)
    }
    if (DisplayFlag == 4 && (millis() - DisplayTimer > 3000)) {
        DisplayFlag = 0;
        if (timeSynched) clockTicker_blink ();                   // отображаем  время
        else display.displayByte(_dash, _dash, _dash, _dash);    // отображаем прочерки
    }   

// Режим "ЧАСЫ / ПОГОДА"
if (DisplayFlag == 0) {
  if (inClockWeatherMode) {
    // ===== ПЕРЕКЛЮЧЕНИЕ =====
    if (showClock) {
      if (millis() - displaySwitchTimer >= CLOCK_SHOW_INTERVAL) {
        displaySwitchTimer = millis();
        showClock = false;
      }
    } else {
      if (weatherErrActive) {
        if (millis() - weatherErrTimer >= WEATHER_ERR_TIME) {
          weatherErrActive = false;
          showClock = true;
          displaySwitchTimer = millis();
        }
      } else {
        if (millis() - displaySwitchTimer >= WEATHER_SHOW_INTERVAL) {
          displaySwitchTimer = millis();
          showClock = true;
        }
      }
    }
    // ===== ОТОБРАЖЕНИЕ =====
    if (showClock) {
      display.displayClock(hours, last_minute);
    } else {
      display.point(false);
      // ---- ПОГОДА ----
      if (currentTemp > -50) {
        weatherErrActive = false;
        int8_t t = round(currentTemp);
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
      } else {
        // ---- ОШИБКА ПОГОДЫ ----
  if (!weatherErrActive) {
    weatherErrActive = true;
    weatherErrTimer = millis();
    weatherErrBlinkTimer = millis();
    weatherErrBlinkState = true;
  }
  // мигание
  if (millis() - weatherErrBlinkTimer >= WEATHER_ERR_BLINK) {
    weatherErrBlinkTimer = millis();
    weatherErrBlinkState = !weatherErrBlinkState;
  }
  if (weatherErrBlinkState) {
    display.displayByte(_empty, _E_, _r_, _r_);
  } else {
    display.displayByte(_empty, _empty, _empty, _empty);
  }
  if (millis() - weatherErrTimer >= WEATHER_ERR_TIME) {
    weatherErrActive = false;
    showClock = true;
    displaySwitchTimer = millis();
  }
      }
    }
  } else {
    clockTicker_blink();
  }
 }
}
#endif  //USE_TM1637
