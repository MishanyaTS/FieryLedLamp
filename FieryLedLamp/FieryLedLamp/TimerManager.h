#pragma once

class TimerManager
{
  public:
    static bool TimerRunning;                               // флаг "таймер взведён"
    static bool TimerHasFired;                              // флаг "таймер отработал"
    static uint8_t TimerOption;                             // индекс элемента в списке List Picker'а
    static uint32_t TimeToFire;                             // время, в которое должен сработать таймер (millis)
    static void HandleTimer(                                // функция, обрабатывающая срабатывание таймера, гасит матрицу
      bool* ONflag,
      void (*changePower)())
    {
      if (!TimerManager::TimerHasFired &&
           TimerManager::TimerRunning &&
           millis() >= TimerManager::TimeToFire)
      {
        #if GENERAL_DEBUG
        LOG.print(F("Выключение по таймеру\n\n"));
        #endif

        TimerManager::TimerRunning = false;
        TimerManager::TimerHasFired = true;
        *ONflag = false;
        jsonWrite(configSetup, "Power", (uint8_t)*ONflag);
        persistEffectSettingsBeforePowerOff();
        changePower();
        #if USE_MULTIPLE_LAMPS_CONTROL
        multiple_lamp_control ();
        #endif  //USE_MULTIPLE_LAMPS_CONTROL        

      }
    }
};
