#if USE_IR_RECEIVER

static bool     IR_LEARN_ACTIVE = false;
static uint32_t IR_LEARN_START  = 0;
static uint32_t IR_LEARN_LAST   = 0;
static char     IR_LEARN_KEY[24] = {0};
static const uint32_t IR_LEARN_TIMEOUT_MS = 15000UL; // 15 сек

static bool IR_IsAllowedKey(const char* k) {
  if (!k || !k[0]) return false;

  const char* allowed[] = {
    "on_off","mute",
    "prev","next","cycle","eq","time","weather",
    "vol_down","vol_up","ip",
    "br_up","br_down","sp_up","sp_down","sc_up","sc_down",
    "fav_add","fav_del","rnd","def",
    "fold_prev","fold_next",
    "d0","d1","d2","d3","d4","d5","d6","d7","d8","d9",
  };
  for (size_t i = 0; i < sizeof(allowed) / sizeof(allowed[0]); i++) {
    if (strcmp(k, allowed[i]) == 0) return true;
  }
  return false;
}

// Старт обучения
bool IR_LearnStart(const char* key) {
  if (!IR_IsAllowedKey(key)) return false;

  memset(IR_LEARN_KEY, 0, sizeof(IR_LEARN_KEY));
  strncpy(IR_LEARN_KEY, key, sizeof(IR_LEARN_KEY) - 1);

  IR_LEARN_ACTIVE = true;
  IR_LEARN_START  = millis();
  IR_LEARN_LAST   = 0;
  return true;
}

void IR_LearnGetStatusJson(String &out) {
  // авто-таймаут
  if (IR_LEARN_ACTIVE && (millis() - IR_LEARN_START > IR_LEARN_TIMEOUT_MS)) {
    IR_LEARN_ACTIVE = false;
    memset(IR_LEARN_KEY, 0, sizeof(IR_LEARN_KEY));
  }

  out = "{";
  out += "\"active\":";
  out += (IR_LEARN_ACTIVE ? "true" : "false");
  out += ",\"key\":\"";
  out += IR_LEARN_KEY;
  out += "\",\"last\":";
  out += String(IR_LEARN_LAST);
  out += "}";
}

// Если активен режим обучения — сохраняем код и НЕ выполняем действие кнопки.
static bool IR_LearnHandle(uint32_t code, uint8_t dataReadyState) {
  if (!IR_LEARN_ACTIVE) return false;
  if (code == 0xFFFFFFFF) return true;      // NEC repeat
  if (dataReadyState == 2) return true;     // repeat state
  if (millis() - IR_LEARN_START > IR_LEARN_TIMEOUT_MS) {
    IR_LEARN_ACTIVE = false;
    memset(IR_LEARN_KEY, 0, sizeof(IR_LEARN_KEY));
    return true;
  }

  if (code == 0) return true;
  String configIR = readFile(F("config_ir.json"), 4096);
  if (configIR == "Failed" || configIR == "Large") {
    IR_LEARN_ACTIVE = false;
    memset(IR_LEARN_KEY, 0, sizeof(IR_LEARN_KEY));
    return true;
  }
  // сохраняем код в выбранный ключ
  jsonWrite(configIR, IR_LEARN_KEY, (int)code);
  writeFile(F("config_ir.json"), configIR);

  IR_LoadConfigFromFile();
  IR_LEARN_LAST = code;
  IR_LEARN_ACTIVE = false;
  memset(IR_LEARN_KEY, 0, sizeof(IR_LEARN_KEY));
  return true;
}

void IR_Receive_Button_Handle() {  // Обработка принятых команд (нажатых кнопок пульта ДУ)
  if (IR_LearnHandle(IR_Code, IR_Data_Ready)) return;
  if (IR_Code == 0xFFFFFFFF) return;   // игнор repeat кадра NEC
  if (IR_Data_Ready == 2) return;      // игнор повтор (repeat state)

    static uint32_t lastCode = 0;
    static uint32_t lastMs   = 0;
    uint32_t now = millis();
    if (IR_Code == lastCode && (now - lastMs) < 200) return;
    lastCode = IR_Code;
    lastMs   = now;

  if (IR_Code == IR_ON_OFF) {
    if (IR_Data_Ready != 2) {
      IR_Power();
    }
  }
  else if (IR_Code == IR_MUTE) {
    if (IR_Data_Ready != 2) {
      Mute();
    }
  }
  else if (IR_Code == IR_PREV) {
    Prev_Next_eff(false);
  }
  else if (IR_Code == IR_NEXT) {
    Prev_Next_eff(true);
  }
  else if (IR_Code == IR_CYCLE) {
    if (IR_Data_Ready != 2) {
     Cycle_on_off();
    }
  }
  else if (IR_Code == IR_EQ) {
    if (IR_Data_Ready != 2) {
     IR_Equalizer();
    }
  }
  else if (IR_Code == IR_TIME) {
    if (IR_Data_Ready != 2) {
     printTime(thisTime, true, ONflag);
    }
  }
  else if (IR_Code == IR_WEATHER) {
    if (IR_Data_Ready != 2) {
     printWeather(thisTime, true, ONflag);
    }
  }
  else if (IR_Code == IR_VOL_DOWN) {
    Volum_Up_Down(false);
  }
  else if (IR_Code == IR_VOL_UP) {
    Volum_Up_Down(true);
  }
  else if (IR_Code == IR_IP) {
    if (IR_Data_Ready != 2) {
     Print_IP();
    }
  }
  else if (IR_Code == IR_BR_UP) {
    Bright_Up_Down(true);
  }
  else if (IR_Code == IR_BR_DOWN) {
    Bright_Up_Down(false);
  }
  else if (IR_Code == IR_SP_UP) {
    Speed_Up_Down(true);
  }
  else if (IR_Code == IR_SP_DOWN) {
    Speed_Up_Down(false);
  }
  else if (IR_Code == IR_SC_UP) {
    Scale_Up_Down(true);
  }
  else if (IR_Code == IR_SC_DOWN) {
    Scale_Up_Down(false);
  }
  else if (IR_Code == IR_FAV_ADD) {
    Favorit_Add_Del(true);
  }
  else if (IR_Code == IR_FAV_DEL) {
      Favorit_Add_Del(false);
  }
  else if (IR_Code == IR_RND) {
    if (IR_Data_Ready != 2) {
      Current_Eff_Rnd_Def(true);
    }
  }
  else if (IR_Code == IR_DEF) {
    if (IR_Data_Ready != 2) {
      Current_Eff_Rnd_Def(false);
    }
  }
  else if (IR_Code == IR_FOLD_PREV) {
      Folder_Next_Prev(false);
  }
  else if (IR_Code == IR_FOLD_NEXT) {
      Folder_Next_Prev(true);
  }
  else if (IR_Code == IR_1) {
    if (IR_Data_Ready != 2) {
      Digit_Handle(1);
    }
  }
  else if (IR_Code == IR_2) {
    if (IR_Data_Ready != 2) {
      Digit_Handle(2);
    }
  }
  else if (IR_Code == IR_3) {
    if (IR_Data_Ready != 2) {
      Digit_Handle(3);
    }
  }
  else if (IR_Code == IR_4) {
    if (IR_Data_Ready != 2) {
      Digit_Handle(4);
    }
  }
  else if (IR_Code == IR_5) {
    if (IR_Data_Ready != 2) {
      Digit_Handle(5);
    }
  }
  else if (IR_Code == IR_6) {
    if (IR_Data_Ready != 2) {
      Digit_Handle(6);
    }
  }
  else if (IR_Code == IR_7) {
    if (IR_Data_Ready != 2) {
      Digit_Handle(7);
    }
  }
  else if (IR_Code == IR_8) {
    if (IR_Data_Ready != 2) {
      Digit_Handle(8);
    }
  }
  else if (IR_Code == IR_9) {
    if (IR_Data_Ready != 2) {
      Digit_Handle(9);
    }
  }
  else if (IR_Code == IR_0) {
    if (IR_Data_Ready != 2) {
      Digit_Handle(0);
    }
  }
  #if GENERAL_DEBUG
  LOG.print("IR_CODE = ");
  LOG.println(IR_Code, HEX);
  LOG.print("IR DEC = ");
  LOG.println(IR_Code);
  #endif  //GENERAL_DEBUG
}

void IR_Power()   {
    if (dawnFlag == 1) {
        #if USE_MP3_PLAYER
        if (alarm_sound_flag) {
           //myDFPlayer.pause();
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
            changePower();
       }
       return;
    }
        else if (sunsetFlag == 1) {
        #if USE_MP3_PLAYER
        if (sunset_sound_flag) {
           //myDFPlayer.pause();
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
            changePower();
       }
       return;
    }
    else
    {
        ONflag = !ONflag;
        jsonWrite(configSetup, "Power", ONflag);
        changePower(); // Сначала выключаем матрицу
        if (!ONflag) {
            timeout_save_file_changes = millis() - SAVE_FILE_DELAY_TIMEOUT;
            if (!FavoritesManager::FavoritesRunning) EepromManager::EepromPut(modes);
            save_file_changes = 7;
            Save_File_Changes();
        }
        else {
            //EepromManager::EepromGet(modes);
            timeout_save_file_changes = millis();
            bitSet(save_file_changes, 0);
        }       
    }
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
    #if USE_MULTIPLE_LAMPS_CONTROL
    if (ONflag) {
        repeat_multiple_lamp_control=true;
    }
    else {
        multiple_lamp_control ();
    }
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
}

void Mute()   {                // Вкл / Откл звука
    #if USE_MP3_PLAYER
    if (mp3_player_connect == 4) {
      if (eff_sound_on) {
        eff_sound_on = 0;
        #if GENERAL_DEBUG
        LOG.println (F("Звук выключен"));
        #endif
      }
      else {
        eff_sound_on = eff_volume;
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
    bitSet (save_file_changes, 0);
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
  #endif  // USE_MP3_PLAYER
}

void Prev_Next_eff(bool direction)   {
    if (ONflag)    
    {
      uint8_t temp = jsonReadtoInt(configSetup, "eff_sel");
      if (direction) {
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
      }
      else {
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
      }
    currentMode = temp;
    jsonWrite(configSetup, "eff_sel", temp);
    jsonWrite(configSetup, "br", modes[currentMode].Brightness);
    jsonWrite(configSetup, "sp", modes[currentMode].Speed);
    jsonWrite(configSetup, "sc", modes[currentMode].Scale);
    SetBrightness(modes[currentMode].Brightness);
    loadingFlag = true;
    //settChanged = true;
    //eepromTimeout = millis();
    #if USE_TM1637
    DisplayFlag = 0;
    Display_Timer();
    #endif
    #if USE_TFT
      DisplayFlag = 0;
      TFT_Display_Timer();
    #endif
    if (random_on && FavoritesManager::FavoritesRunning)
        selectedSettings = 1U;
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

void Cycle_on_off()   {
    if (ONflag)   {
        uint8_t tmp;
        jsonReadtoInt(configSetup, "cycle_on") == 0 ? tmp = 1 : tmp = 0;
        jsonWrite(configSetup, "cycle_on", tmp);
        FavoritesManager::FavoritesRunning = tmp;
        if (tmp){
            showWarning(CRGB::Blue, 500, 250U);        // мигание синим цветом 0.5 секунды
            EepromManager::EepromPut(modes);
            //eepromTimeout = millis() - EEPROM_WRITE_DELAY;
        }
        else {
            showWarning(CRGB::Red, 500, 250U);        // мигание красным цветом 0.5 секунды
            //EepromManager::EepromGet(modes);
        }
        #if USE_MQTT
        if (espMode == 1U)
        {
          MqttManager::needToPublish = true;
        }
        #endif
    }
}

void Bright_Up_Down(bool direction)   {
    uint8_t delta = IR_Data_Ready == 1 ? 1U : 4U;
    modes[currentMode].Brightness = constrain(direction ? modes[currentMode].Brightness + delta : modes[currentMode].Brightness - delta, 1, 255);
    jsonWrite(configSetup, "br", modes[currentMode].Brightness);
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
    //timeout_save_file_changes = millis();
    //bitSet (save_file_changes, 0);
    //settChanged = true;
    //eepromTimeout = millis();
    #if USE_MULTIPLE_LAMPS_CONTROL
        repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
    #if USE_MQTT
    if (espMode == 1U)
    {
        MqttManager::needToPublish = true;
    }
    #endif
}

void Speed_Up_Down(bool direction)   {
    uint8_t delta = IR_Data_Ready == 1 ? 1U : 4U;
    modes[currentMode].Speed = constrain(direction ? modes[currentMode].Speed + delta : modes[currentMode].Speed - delta, 1, 255);
    jsonWrite(configSetup, "sp", modes[currentMode].Speed);
    loadingFlag = true; // без перезапуска эффекта ничего и не увидишь
    #if USE_TM1637
    DisplayFlag = 3;
    Display_Timer(modes[currentMode].Speed);
    #endif
    #if USE_TFT
      DisplayFlag = 3;
      TFT_Display_Timer(modes[currentMode].Speed);
    #endif
    #if GENERAL_DEBUG
        LOG.printf_P(PSTR("Новое значение скорости: %d\n"), modes[currentMode].Speed);
    #endif
    //timeout_save_file_changes = millis();
    //bitSet (save_file_changes, 0);
    //settChanged = true;
    //eepromTimeout = millis();
    #if USE_MULTIPLE_LAMPS_CONTROL
        repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
    #if USE_MQTT
    if (espMode == 1U)
    {
        MqttManager::needToPublish = true;
    }
    #endif
}

void Scale_Up_Down(bool direction)   {
    uint8_t delta = IR_Data_Ready == 1 ? 1U : 2U;
    modes[currentMode].Scale = constrain(direction ? modes[currentMode].Scale + delta : modes[currentMode].Scale - delta, 1, 100);
    jsonWrite(configSetup, "sc", modes[currentMode].Scale);
    loadingFlag = true; // без перезапуска эффекта ничего и не увидишь
    #if USE_TM1637
    DisplayFlag = 3;
    Display_Timer(modes[currentMode].Scale);
    #endif
    #if USE_TFT
      DisplayFlag = 3;
      TFT_Display_Timer(modes[currentMode].Scale);
    #endif
    
    #if GENERAL_DEBUG
        LOG.printf_P(PSTR("Новое значение масштаба: %d\n"), modes[currentMode].Scale);
    #endif
    #if USE_MULTIPLE_LAMPS_CONTROL
        repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
    #if USE_MQTT
    if (espMode == 1U)
    {
        MqttManager::needToPublish = true;
    }
    #endif
}

void Volum_Up_Down (bool direction)   {
    #if USE_MP3_PLAYER
    eff_volume = constrain(direction ? eff_volume + 1 : eff_volume - 1, 1, 30);
    jsonWrite(configSetup, "vol", eff_volume);
    if (!dawnflag_sound) send_command(6,FEEDBACK,0,eff_volume); //Громкость
    #if USE_TM1637
    DisplayFlag = 3;
    Display_Timer(eff_volume);
    #endif
    #if USE_TFT
      DisplayFlag = 3;
      TFT_Display_Timer(eff_volume);
    #endif
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
    #endif  // USE_MP3_PLAYER
}

void Print_IP()   {
    #if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)      // установка сигнала в пин, управляющий MOSFET транзистором, матрица должна быть включена на время вывода текста
      digitalWrite(MOSFET_PIN, MOSFET_LEVEL);
    #endif
    if (espMode == 1U)
    {
      loadingFlag = true;
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

void Folder_Next_Prev(bool direction)    {
    #if USE_MP3_PLAYER
    if (true) { //(!pause_on && !mp3_stop && eff_sound_on) {
    CurrentFolder = constrain(direction ? CurrentFolder + 1 : CurrentFolder - 1, 0, 99);
    jsonWrite(configSetup, "fold_sel", CurrentFolder);
    if (!pause_on && !mp3_stop && eff_sound_on) {
      send_command(0x17,FEEDBACK,0,CurrentFolder);           // Включить непрерывное воспроизведение указанной папки
      delay(mp3_delay);
    }
    }
    #if USE_TM1637
    DisplayFlag = 0;
    Display_Timer();
    #endif
    #if USE_TFT
      DisplayFlag = 0;
      TFT_Display_Timer();
    #endif
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
    #endif  // USE_MP3_PLAYER
}

void Current_Eff_Rnd_Def(bool direction)   {
    if (direction) {
    selectedSettings = 1U;
    updateSets();
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
    }
    else {
    setModeSettings();
    updateSets();    
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
    }
    if (direction) showWarning(CRGB::Blue, 500, 250U);  // мигание синим цветом 0.5 секунды
    else showWarning(CRGB::Red, 500, 250U);             // мигание красным цветом 0.5 секунды
}

void IR_Equalizer()   {     // Устанавливаем эквалайзер
    #if USE_MP3_PLAYER
    Equalizer++;
    if (Equalizer > 5) Equalizer = 0;
    jsonWrite(configSetup, "eq", Equalizer);
    send_command(0x07, FEEDBACK, 0, Equalizer); 
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    #if USE_TM1637
    DisplayFlag = 3;
    Display_Timer(Equalizer);
    #endif
    #if USE_TFT
      DisplayFlag = 3;
      TFT_Display_Timer(Equalizer);
    #endif
    #endif  // USE_MP3_PLAYER
}

void Favorit_Add_Del(bool direction)   {
    String configCycle = readFile(F("config_cycle.json"), 2048);
    String e = "e" + String (currentMode);
    jsonWrite(configCycle, e, direction ? 1 : 0);
    FavoritesManager::FavoriteModes[currentMode] = (direction ? 1 : 0);
    //writeFile("config_cycle.json", configCycle );
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 2);
    if (direction) showWarning(CRGB::Blue, 500, 250U);  // мигание синим цветом 0.5 секунды
    else showWarning(CRGB::Red, 500, 250U);             // мигание красным цветом 0.5 секунды
}

void Digit_Handle(uint8_t digit) {
    if (Enter_Digits_Count == 0) {
        Enter_Number = digit;
    } else {
        Enter_Number = Enter_Number * 10 + digit;
    }
    Enter_Digits_Count++;
    IR_Digit_Timer = millis();

    #if USE_TM1637
      DisplayFlag = 3;
      Display_Timer(Enter_Number);
    #endif
    #if USE_TFT
      DisplayFlag = 3;
      TFT_Display_Timer(Enter_Number);
    #endif
    // если ввели 3 цифры — применяем сразу
    if (Enter_Digits_Count >= 3) {
        Apply_Entered_Effect();
    }
  }
  
void Apply_Entered_Effect() {
    if (Enter_Digits_Count == 0) return;
    if (Enter_Number >= MODE_AMOUNT) {
        showWarning(CRGB::Red, 700, 200);
        Enter_Digits_Count = 0;
        Enter_Number = 0;
        return;
    }
    currentMode = Enter_Number;
    jsonWrite(configSetup, "eff_sel", Enter_Number);
    jsonWrite(configSetup, "br", modes[currentMode].Brightness);
    jsonWrite(configSetup, "sp", modes[currentMode].Speed);
    jsonWrite(configSetup, "sc", modes[currentMode].Scale);
    SetBrightness(modes[currentMode].Brightness);
    loadingFlag = true;
    #if USE_MQTT
      if (espMode == 1U) MqttManager::needToPublish = true;
    #endif
    #if USE_BLYNK
      updateRemoteBlynkParams();
    #endif
    #if USE_MULTIPLE_LAMPS_CONTROL
      repeat_multiple_lamp_control = true;
    #endif
    Enter_Digits_Count = 0;
    Enter_Number = 0;
}

uint32_t IR_ON_OFF = 0;
uint32_t IR_MUTE = 0;
uint32_t IR_PREV = 0;
uint32_t IR_NEXT = 0;
uint32_t IR_CYCLE = 0;
uint32_t IR_EQ = 0;
uint32_t IR_TIME = 0;
uint32_t IR_WEATHER = 0;
uint32_t IR_VOL_DOWN = 0;
uint32_t IR_VOL_UP = 0;
uint32_t IR_IP = 0;
uint32_t IR_BR_UP = 0;
uint32_t IR_BR_DOWN = 0;
uint32_t IR_SP_UP = 0;
uint32_t IR_SP_DOWN = 0;
uint32_t IR_SC_UP = 0;
uint32_t IR_SC_DOWN = 0;
uint32_t IR_FAV_ADD = 0;
uint32_t IR_FAV_DEL = 0;
uint32_t IR_RND = 0;
uint32_t IR_DEF = 0;
uint32_t IR_FOLD_PREV = 0;
uint32_t IR_FOLD_NEXT = 0;
uint32_t IR_1 = 0;
uint32_t IR_2 = 0;
uint32_t IR_3 = 0;
uint32_t IR_4 = 0;
uint32_t IR_5 = 0;
uint32_t IR_6 = 0;
uint32_t IR_7 = 0;
uint32_t IR_8 = 0;
uint32_t IR_9 = 0;
uint32_t IR_0 = 0;

void IR_LoadConfigFromFile() {
  String configIR = readFile(F("config_ir.json"), 4096);
  if (configIR == "Failed" || configIR == "Large") return;

  IR_ON_OFF   = jsonReadtoInt(configIR, "on_off");
  IR_MUTE     = jsonReadtoInt(configIR, "mute");
  IR_PREV     = jsonReadtoInt(configIR, "prev");
  IR_NEXT     = jsonReadtoInt(configIR, "next");
  IR_CYCLE    = jsonReadtoInt(configIR, "cycle");
  IR_EQ       = jsonReadtoInt(configIR, "eq");
  IR_TIME     = jsonReadtoInt(configIR, "time");
  IR_WEATHER  = jsonReadtoInt(configIR, "weather");
  IR_VOL_DOWN = jsonReadtoInt(configIR, "vol_down");
  IR_VOL_UP   = jsonReadtoInt(configIR, "vol_up");
  IR_IP       = jsonReadtoInt(configIR, "ip");
  IR_BR_UP    = jsonReadtoInt(configIR, "br_up");
  IR_BR_DOWN  = jsonReadtoInt(configIR, "br_down");
  IR_SP_UP    = jsonReadtoInt(configIR, "sp_up");
  IR_SP_DOWN  = jsonReadtoInt(configIR, "sp_down");
  IR_SC_UP    = jsonReadtoInt(configIR, "sc_up");
  IR_SC_DOWN  = jsonReadtoInt(configIR, "sc_down");
  IR_FAV_ADD  = jsonReadtoInt(configIR, "fav_add");
  IR_FAV_DEL  = jsonReadtoInt(configIR, "fav_del");
  IR_RND      = jsonReadtoInt(configIR, "rnd");
  IR_DEF      = jsonReadtoInt(configIR, "def");
  IR_FOLD_PREV = jsonReadtoInt(configIR, "fold_prev");
  IR_FOLD_NEXT = jsonReadtoInt(configIR, "fold_next");
  IR_1 = jsonReadtoInt(configIR, "d1");
  IR_2 = jsonReadtoInt(configIR, "d2");
  IR_3 = jsonReadtoInt(configIR, "d3");
  IR_4 = jsonReadtoInt(configIR, "d4");
  IR_5 = jsonReadtoInt(configIR, "d5");
  IR_6 = jsonReadtoInt(configIR, "d6");
  IR_7 = jsonReadtoInt(configIR, "d7");
  IR_8 = jsonReadtoInt(configIR, "d8");
  IR_9 = jsonReadtoInt(configIR, "d9");
  IR_0 = jsonReadtoInt(configIR, "d0");
}
#endif //USE_IR_RECEIVER
