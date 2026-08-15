#if USE_BUTTON

bool brightDirection;
static bool startButtonHolding = false;                     // флаг: кнопка удерживается для изменения яркости/скорости/масштаба лампы кнопкой
static bool Button_Holding = false;

void buttonTick()
{
  if (!buttonEnabled) // события кнопки не обрабатываются, если она заблокирована , но обрабатывается сброс в default
  {
    touch.tick();
    if (touch.isStep() && touch.getHoldClicks() == 14U) {
        LOG.println("\n*** Reset to Default ***");
        showWarning(CRGB::Red, 500, 250U);
        esp_task_wdt_reset();
        resetCurrentEffectToDefaults();
        updateSets();    
        if(FileCopy (F("/default/config.json"), F("/config.json"))) {
            esp_task_wdt_reset();
            showWarning(CRGB::Green, 500, 250U);
            ESP.restart();
        }
        else {
            esp_task_wdt_reset();
            showWarning(CRGB::Red, 500, 250U);
        }
    }  
    return;
  }

  touch.tick();

  uint8_t clickCount = 0U;
  if (!touch.isHold() && !startButtonHolding)
  {
    clickCount = touch.hasClicks() ? touch.getClicks() : 0U;
  }

  // однократное нажатие
  if (clickCount == btn_click_power)
  {
    if (dawnFlag == 1) {
        #if USE_MP3_PLAYER
        if (alarm_sound_flag) {
           send_command(0x0E,0,0,0); //Пауза
           mp3_stop = true;
           alarm_sound_flag = false;
        }
        else
        #endif  // USE_MP3_PLAYER
        {
            manualOff = true;
            dawnFlag = 2;
            #if USE_TM1637
            clockTicker_blink();
            #endif
            SetBrightness(modes[currentMode].Brightness);
            saveEffectSettingsNow(false);
            changePower();
       }
       return;
    }
    else if (sunsetFlag == 1) {
        #if USE_MP3_PLAYER
        if (sunset_sound_flag) {
           send_command(0x0E,0,0,0); //Пауза
           mp3_stop = true;
           sunset_sound_flag = false;
        }
        else
        #endif  // USE_MP3_PLAYER
        {
            manualsOff = true;
            sunsetFlag = 2;
            #if USE_TM1637
            clockTicker_blink();
            #endif
            SetBrightness(modes[currentMode].Brightness);
            saveEffectSettingsNow(false);
            changePower();
       }
       return;
    }
    else
    {
      ONflag = !ONflag;
      jsonWrite(configSetup, "Power", ONflag);
      if (!ONflag)  {
        persistEffectSettingsBeforePowerOff();
        changePower();
      }
      else {
        restoreEffectSettingsForPowerOn();
        timeout_save_file_changes = millis();
        bitSet (save_file_changes, 0);
        changePower();
      }
    }

    #if USE_MQTT
    if (espMode == 1U)
    {
      MqttManager::needToPublish = true;
    }
    #endif
    #if USE_BLYNK
    updateRemoteBlynkParams();
    #endif
    #if USE_MULTIPLE_LAMPS_CONTROL
    if (ONflag) {
        repeat_multiple_lamp_control = true;
    }
    else {
        multiple_lamp_control ();
    }
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
  }

  // двухкратное нажатие
  if (clickCount == btn_click_next){
     #if USE_MP3_PLAYER
     if (dawnFlag == 1) {            //if (dawnFlag && alarm_sound_flag) {
        //myDFPlayer.pause();
        send_command(0x0E,0,0,0);  //Пауза
        mp3_stop = true;
        alarm_sound_flag = false;
        manualOff = true;
        dawnFlag = 2;
        #if USE_TM1637
        clockTicker_blink();
        #endif
        SetBrightness(modes[currentMode].Brightness);
        changePower();
       }
       else if (sunsetFlag == 1) {
        //myDFPlayer.pause();
        send_command(0x0E,0,0,0);  //Пауза
        mp3_stop = true;
        sunset_sound_flag = false;
        manualsOff = true;
        sunsetFlag = 2;
        #if USE_TM1637
        clockTicker_blink();
        #endif
        SetBrightness(modes[currentMode].Brightness);
        changePower();
       }
       else
       #endif  // USE_MP3_PLAYER
      
  if (ONflag)    
  {
    uint8_t temp = jsonReadtoInt(configSetup, "eff_sel");
    if (Favorit_only)
    {
      uint8_t lastMode = currentMode;
      do 
      {
        if (++temp >= MODE_AMOUNT) temp = 0;
        currentMode = temp;
      } while (FavoritesManager::FavoriteModes[currentMode] == 0 && currentMode != lastMode);
      if (currentMode == lastMode) // если ни один режим не добавлен в избранное, всё равно куда-нибудь переключимся
        if (++temp >= MODE_AMOUNT) temp = 0;
        currentMode = temp;
    }
    else
      if (++temp >= MODE_AMOUNT) temp = 0;
    currentMode = temp;
    jsonWrite(configSetup, "eff_sel", temp);
    jsonWrite(configSetup, "br", modes[currentMode].Brightness);
    jsonWrite(configSetup, "sp", modes[currentMode].Speed);
    jsonWrite(configSetup, "sc", modes[currentMode].Scale);
    SetBrightness(modes[currentMode].Brightness);
    loadingFlag = true;

      if (random_on && FavoritesManager::FavoritesRunning)
      {
        selectedSettings = 1U;
        applyPendingRandomEffectSettings();
      }

    #if USE_MQTT
    if (espMode == 1U)
    {
      MqttManager::needToPublish = true;
    }
    #endif
    #if USE_BLYNK
    updateRemoteBlynkParams();
    #endif
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
  }
  }

  // трёхкратное нажатие
  if (ONflag && clickCount == btn_click_prev)
  {
    uint8_t temp = jsonReadtoInt(configSetup, "eff_sel");
    if (Favorit_only) 
    {
      uint8_t lastMode = currentMode;
      do
      {
        if (--temp >= MODE_AMOUNT) temp = MODE_AMOUNT - 1;
        currentMode = temp;
      } while (FavoritesManager::FavoriteModes[currentMode] == 0 && currentMode != lastMode);
      if (currentMode == lastMode) // если ни один режим не добавлен в избранное, всё равно куда-нибудь переключимся
        if (--temp >= MODE_AMOUNT) temp = MODE_AMOUNT - 1;
        currentMode = temp;
    }
    else 
      if (--temp >= MODE_AMOUNT) temp = MODE_AMOUNT - 1;
    currentMode = temp;
    jsonWrite(configSetup, "eff_sel", temp);
    jsonWrite(configSetup, "br", modes[currentMode].Brightness);
    jsonWrite(configSetup, "sp", modes[currentMode].Speed);
    jsonWrite(configSetup, "sc", modes[currentMode].Scale);
    SetBrightness(modes[currentMode].Brightness);
    loadingFlag = true;

      if (random_on && FavoritesManager::FavoritesRunning)
      {
        selectedSettings = 1U;
        applyPendingRandomEffectSettings();
      }

    #if USE_MQTT
    if (espMode == 1U)
    {
      MqttManager::needToPublish = true;
    }
    #endif
    #if USE_BLYNK
    updateRemoteBlynkParams();
    #endif
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
  }

  // четырёхкратное нажатие
  if (clickCount == btn_click_action4)
  {
    #if USE_OTA
    if (otaManager.RequestOtaUpdate())
    {
      if (!ONflag) restoreEffectSettingsForPowerOn();
      ONflag = true;
      jsonWrite(configSetup, "Power", ONflag);
      currentMode = EFF_MATRIX;                             // принудительное включение режима "Матрица" для индикации перехода в режим обновления по воздуху
      jsonWrite(configSetup, "eff_sel", currentMode);
      jsonWrite(configSetup, "br", modes[currentMode].Brightness);
      jsonWrite(configSetup, "sp", modes[currentMode].Speed);
      jsonWrite(configSetup, "sc", modes[currentMode].Scale);
      changePower();
    }
    else
    #endif
    
    #ifdef BUTTON_CAN_SET_SLEEP_TIMER
    //if (!ota)
    {
      if (!dawnFlag) {
        // мигать об успехе операции лучше до вызова changePower(), иначе сперва мелькнут кадры текущего эффекта
        showWarning(CRGB::Blue, 1000, 250U);                    // мигание синим цветом 1 секунду
        if (!ONflag) restoreEffectSettingsForPowerOn();
        ONflag = true;
        changePower();
        jsonWrite(configSetup, "Power", ONflag);
        jsonWrite(configSetup, "tmr", 1);
        #if USE_BLYNK
        updateRemoteBlynkParams();
        #endif
        TimerManager::TimeToFire = millis() + BUTTON_SET_SLEEP_TIMER1 * 60UL * 1000UL;
        TimerManager::TimerRunning = true;
      }
    }
    #endif //BUTTON_CAN_SET_SLEEP_TIMER 
    ;
  }

  // пятикратное нажатие
  if (clickCount == btn_click_ip)                                     // вывод IP на лампу
  {
    if (espMode == 1U)
    {
      loadingFlag = true;
      
      #if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)      // установка сигнала в пин, управляющий MOSFET транзистором, матрица должна быть включена на время вывода текста
      digitalWrite(MOSFET_PIN, MOSFET_LEVEL);
      #endif
      #if USE_TFT
      TFT_ShowIP(WiFi.localIP().toString().c_str());
      #endif
      while(!fillString(WiFi.localIP().toString().c_str(), CRGB::White, false)) {
          delay(1);
          esp_task_wdt_reset();
          }
      #if USE_TFT
      TFT_HideIP();
      #endif
      if (ColorTextFon  & (!ONflag || (currentMode == EFF_COLOR && modes[currentMode].Scale < 3))){
        FastLED.clear();
        delay(1);
        FastLED.show();
      }
      #if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)      // возвращаем MOSFET в состояние лампы/рассвета/заката
      updateMosfetState();
      #endif

      loadingFlag = true;
    }
  else
    {
      loadingFlag = true;
      #if USE_TFT
      TFT_ShowIP(WiFi.softAPIP().toString().c_str());
      #endif
      String str = "Точка доступа 192.168.4.1";
      while(!fillString(str.c_str(), CRGB::White, false)) {
          delay(1);
          esp_task_wdt_reset();
          }
      #if USE_TFT
      TFT_HideIP();
      #endif
      if (ColorTextFon  & (!ONflag || (currentMode == EFF_COLOR && modes[currentMode].Scale < 3))){
        FastLED.clear();
        delay(1);
        FastLED.show();
      }
      loadingFlag = true;
    }
    #if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)      // возвращаем MOSFET в состояние лампы/рассвета/заката
      updateMosfetState();
    #endif
}

  // шестикратное нажатие
  if (clickCount == btn_click_time)                                     // вывод текущего времени бегущей строкой
  {
    printTime(thisTime, true, ONflag);
  }


  // семикратное нажатие
  if (clickCount == btn_click_esp_mode)  // if (ONflag &&                   // смена рабочего режима лампы: с WiFi точки доступа на WiFi клиент или наоборот
  {
    #ifdef RESET_WIFI_ON_ESP_MODE_CHANGE
      if (espMode) wifiManager.resetSettings();                             // сброс сохранённых SSID и пароля (сброс настроек подключения к роутеру)
    #endif
    espMode = (espMode == 0U) ? 1U : 0U;
    jsonWrite(configSetup, "ESP_mode", (int)espMode);
    saveConfig();  

    #if GENERAL_DEBUG
    LOG.print(F("Рабочий режим лампы изменён и сохранён в энергонезависимую память\nНовый рабочий режим: ESP_MODE ="));
    LOG.print(espMode);
    LOG.print(espMode == 0U ? F("WiFi точка доступа") : F("WiFi клиент (подключение к роутеру)"));
    LOG.print("\nРестарт...\n");
    delay(1000);
    #endif

    showWarning(CRGB::Red, 3000U, 500U);                    // мигание красным цветом 3 секунды - смена рабочего режима лампы, перезагрузка
    ESP.restart();
  }

  #if USE_MP3_PLAYER
  
  // Восьмикратное нажатие
  if (clickCount == btn_click_sound)  {                                  // Вкл / Откл звука
    if (mp3_player_connect == 4) {
      if (eff_sound_on) {
        eff_sound_on = 0;
        showWarning(CRGB::Yellow, 1000, 250U);                    // мигание желтым цветом 1 секунду
        #if GENERAL_DEBUG
        LOG.println (F("Звук выключен"));
        #endif
      }
      else {
        eff_sound_on = eff_volume;
        showWarning(CRGB::Blue, 1000, 250U);                    // мигание синим цветом 1 секунду
        #if GENERAL_DEBUG
        LOG.println (F("Звук включен"));
        #endif
      }
    }
    else  {
        showWarning(CRGB::Red, 1000, 250U);                    // мигание красным цветом 1 секунду
        #if GENERAL_DEBUG
        LOG.println (F("mp3 player не подключен"));
        #endif
    }
    jsonWrite(configSetup, "on_sound", eff_sound_on > 0 ? 1 : 0);
    timeout_save_file_changes = millis();
    bitSet(save_file_changes, 0);
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
  }
  #endif  // USE_MP3_PLAYER

  // девятикратное нажатие
  if (clickCount == btn_click_weather)                                     // вывод текущей погоды бегущей строкой
  {
    printWeather(thisTime, true, ONflag);
  }

  // кнопка только начала удерживаться
  if (touch.isHolded()) // пускай для выключенной лампы удержание кнопки включает белую лампу
  {
    brightDirection = !brightDirection;
    startButtonHolding = true;
  }

  // кнопка нажата и удерживается
if (touch.isStep()){
  if (ONflag && !Button_Holding)
  {

    int8_t but = touch.getHoldClicks();
        //Serial.println (but);

    switch (but )
    {
      case 0U:                                              // просто удержание (до удержания кнопки кликов не было) - изменение яркости
      {
        uint8_t delta = modes[currentMode].Brightness < 10U // определение шага изменения яркости: при яркости [1..9] шаг = 1, при [10..100] шаг = 5
          ? 1U
          : 5U;
        modes[currentMode].Brightness =
          constrain(brightDirection
            ? modes[currentMode].Brightness + delta
            : modes[currentMode].Brightness - delta,
          1, EFFECT_BRIGHTNESS_MAX);
        jsonWrite(configSetup, "br", modes[currentMode].Brightness);
        markEffectSettingsChanged();
        SetBrightness(modes[currentMode].Brightness);
        #if USE_TM1637
        DisplayFlag = 3;
        Display_Timer(modes[currentMode].Brightness);
        #endif
        #if USE_TFT
          DisplayFlag = 3;
          TFT_Display_Timer(modes[currentMode].Brightness);
        #endif

        #if GENERAL_DEBUG
        LOG.printf_P(PSTR("Новое значение яркости: %d\n"), modes[currentMode].Brightness);
        #endif
        #if USE_MULTIPLE_LAMPS_CONTROL
        repeat_multiple_lamp_control = true;
        #endif  //USE_MULTIPLE_LAMPS_CONTROL
        break;
      }

      case 1U:                                              // удержание после одного клика - изменение скорости
      {
        modes[currentMode].Speed = constrain(brightDirection ? modes[currentMode].Speed + 1 : modes[currentMode].Speed - 1, 1, 255);
        jsonWrite(configSetup, "sp", modes[currentMode].Speed);
        markEffectSettingsChanged();
        loadingFlag = true; // без перезапуска эффекта ничего и не увидишь

        #if GENERAL_DEBUG
        LOG.printf_P(PSTR("Новое значение скорости: %d\n"), modes[currentMode].Speed);
        #endif
        #if USE_MULTIPLE_LAMPS_CONTROL
        repeat_multiple_lamp_control = true;
        #endif  //USE_MULTIPLE_LAMPS_CONTROL
        #if USE_TM1637
        DisplayFlag = 3;
        Display_Timer(modes[currentMode].Speed);
        #endif
        #if USE_TFT
          DisplayFlag = 3;
          TFT_Display_Timer(modes[currentMode].Speed);
        #endif
        #if USE_MULTIPLE_LAMPS_CONTROL
        repeat_multiple_lamp_control = true;
        #endif  //USE_MULTIPLE_LAMPS_CONTROL
        break;
      }

      case 2U:                                              // удержание после двух кликов - изменение масштаба
      {
        modes[currentMode].Scale = constrain(
            brightDirection ? modes[currentMode].Scale + 1 : modes[currentMode].Scale - 1,
            1, effectScaleStepMaximum(currentMode));
        jsonWrite(configSetup, "sc", modes[currentMode].Scale);
        markEffectSettingsChanged();
        loadingFlag = true; // без перезапуска эффекта ничего и не увидишь

        #if GENERAL_DEBUG
        LOG.printf_P(PSTR("Новое значение масштаба: %d\n"), modes[currentMode].Scale);
        #endif
        #if USE_MULTIPLE_LAMPS_CONTROL
        repeat_multiple_lamp_control = true;
        #endif  //USE_MULTIPLE_LAMPS_CONTROL
        #if USE_TM1637
        DisplayFlag = 3;
        Display_Timer(modes[currentMode].Scale);
        #endif
        #if USE_TFT
          DisplayFlag = 3;
          TFT_Display_Timer(modes[currentMode].Scale);
        #endif
        #if USE_MULTIPLE_LAMPS_CONTROL
        repeat_multiple_lamp_control = true;
        #endif  //USE_MULTIPLE_LAMPS_CONTROL
        break;
      }
      
        #ifdef BUTTON_CAN_SET_SLEEP_TIMER
      case 3U:                                               // Таймер сна 10 мин.
      {
        if(!dawnFlag){
          Button_Holding = true;
          // мигать об успехе операции лучше до вызова changePower(), иначе сперва мелькнут кадры текущего эффекта
          showWarning(CRGB::Blue, 1500U, 250U);                    // мигание синим цветом 1 секунду
          if (!ONflag) restoreEffectSettingsForPowerOn();
          ONflag = true;
          changePower();
          jsonWrite(configSetup, "Power", ONflag);
          jsonWrite(configSetup, "tmr", 1);
          #if USE_BLYNK
          updateRemoteBlynkParams();
          #endif
          TimerManager::TimeToFire = millis() + BUTTON_SET_SLEEP_TIMER2 * 60UL * 1000UL;
          TimerManager::TimerRunning = true;
          break;
        }
        break;
      }
        #endif //BUTTON_CAN_SET_SLEEP_TIMER
        
      case 14U:                                             // Сброс основных настроек, хранящихся в файле config.json
      {
          showWarning(CRGB::Red, 500, 250U);
          esp_task_wdt_reset();
          resetCurrentEffectToDefaults();
          updateSets();    
          if(FileCopy (F("/default/config.json"), F("/config.json"))){
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 2500, 250U);
              ESP.restart();
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 2500, 250U);
          }
          break;
      }
      
      case 19U:                                            // Сброс всех настроек в "заводские"
      {
          showWarning(CRGB::Red, 500, 250U);
          esp_task_wdt_reset();
          resetAllEffectsToDefaults();
          updateSets();    
          if(FileCopy (F("/default/config.json"), F("/config.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }
          if(FileCopy (F("/default/config_cycle.json"), F("/config_cycle.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }
          if(FileCopy (F("/default/config_sound.json"), F("/config_sound.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }
          if(FileCopy (F("/default/config_alarm.json"), F("/config_alarm.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }
          if(FileCopy (F("/default/config_sunset.json"), F("/config_sunset.json"))) {
             esp_task_wdt_reset();
            showWarning(CRGB::Green, 500, 250U);
        }
        else {
            esp_task_wdt_reset();
            showWarning(CRGB::Red, 500, 250U);
        }
          if(FileCopy (F("/default/config_display.json"), F("/config_display.json"))) {
              esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
              esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }

          if(FileCopy (F("/default/config_hardware.json"), F("/config_hardware.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }
          if(FileCopy (F("/default/config_multilamp.json"), F("/config_multilamp.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }
          if(FileCopy (F("/default/config_ip.json"), F("/config_ip.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }
          if(FileCopy (F("/default/config_mqtt.json"), F("/config_mqtt.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }
          ESP.restart();
          break;
      }

      default:
        break;
    }       
  }
  else
  {
  if (!Button_Holding ) {
    int8_t but = touch.getHoldClicks();
        //Serial.println (but);

    switch (but )
    {
      case 0U:                                              // просто удержание (до удержания кнопки кликов не было) - белый свет
      {
        Button_Holding = true;
        if (!ONflag) restoreEffectSettingsForPowerOn();
        currentMode = EFF_WHITE_COLOR;
    jsonWrite(configSetup, "eff_sel", currentMode);
        jsonWrite(configSetup, "br", modes[currentMode].Brightness);
        jsonWrite(configSetup, "sp", modes[currentMode].Speed);
        jsonWrite(configSetup, "sc", modes[currentMode].Scale);
        ONflag = true;
        jsonWrite(configSetup, "Power", ONflag);
        changePower();
        #if USE_BLYNK
        updateRemoteBlynkParams();
        #endif
        break;
      }
        #ifdef BUTTON_CAN_SET_SLEEP_TIMER     
      case 3U:
      {
        if(!dawnFlag){
          Button_Holding = true;
          // мигать об успехе операции лучше до вызова changePower(), иначе сперва мелькнут кадры текущего эффекта
          showWarning(CRGB::Blue, 1500U, 250U);                    // мигание синим цветом 1 секунду
          if (!ONflag) restoreEffectSettingsForPowerOn();
          ONflag = true;
          changePower();
          jsonWrite(configSetup, "Power", ONflag);
          jsonWrite(configSetup, "tmr", 1);
          #if USE_BLYNK
          updateRemoteBlynkParams();
          #endif
          TimerManager::TimeToFire = millis() + BUTTON_SET_SLEEP_TIMER2 * 60UL * 1000UL;
          TimerManager::TimerRunning = true;
          break;
        }
        break;
      }
        #endif //BUTTON_CAN_SET_SLEEP_TIMER
        
      case 14U:
      {
          showWarning(CRGB::Red, 500, 250U);
          esp_task_wdt_reset();
          if(FileCopy (F("/default/config.json"), F("/config.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 2500, 250U);
              ESP.restart();
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 2500, 250U);
          }
          break;
      }
      case 19U:
      {
          showWarning(CRGB::Red, 500, 250U);
          esp_task_wdt_reset();
          if(FileCopy (F("/default/config.json"), F("/config.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }
          if(FileCopy (F("/default/config_cycle.json"), F("/config_cycle.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }
          if(FileCopy (F("/default/config_sound.json"), F("/config_sound.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }
          if(FileCopy (F("/default/config_alarm.json"), F("/config_alarm.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }
          if(FileCopy (F("/default/config_sunset.json"), F("/config_sunset.json"))) {
             esp_task_wdt_reset();
            showWarning(CRGB::Green, 500, 250U);
        }
        else {
            esp_task_wdt_reset();
            showWarning(CRGB::Red, 500, 250U);
        }
          if(FileCopy (F("/default/config_display.json"), F("/config_display.json"))) {
              esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
              esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }

          if(FileCopy (F("/default/config_hardware.json"), F("/config_hardware.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }
          if(FileCopy (F("/default/config_multilamp.json"), F("/config_multilamp.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
            showWarning(CRGB::Red, 500, 250U);
          }
          if(FileCopy (F("/default/config_ip.json"), F("/config_ip.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
              showWarning(CRGB::Red, 500, 250U);
          }
          if(FileCopy (F("/default/config_mqtt.json"), F("/config_mqtt.json"))) {
            esp_task_wdt_reset();
              showWarning(CRGB::Green, 500, 250U);
          }
          else {
            esp_task_wdt_reset();
            showWarning(CRGB::Red, 500, 250U);
          }
          ESP.restart();
          break;
      }
      
    }
   }
  }
  }

  // кнопка отпущена после удерживания
  if (ONflag && !touch.isHold() && startButtonHolding)      // кнопка отпущена после удерживания, нужно отправить MQTT сообщение об изменении яркости лампы
  {
    startButtonHolding = false;
    Button_Holding = false;
    loadingFlag = true;

    #if USE_MQTT
    if (espMode == 1U)
    {
      MqttManager::needToPublish = true;
    }
    #endif
    
    #if USE_BLYNK
    updateRemoteBlynkParams();
    #endif
    
  }
}
#endif
