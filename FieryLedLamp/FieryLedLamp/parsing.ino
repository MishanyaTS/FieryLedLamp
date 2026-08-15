void parseUDP()
{
  int32_t packetSize = Udp.parsePacket();

  if (packetSize)
  {
    int16_t n = Udp.read(packetBuffer, MAX_UDP_BUFFER_SIZE - 1);
    if (n <= 0) return;
    packetBuffer[n] = '\0';
    strlcpy(inputBuffer, packetBuffer, sizeof(inputBuffer));

    #if GENERAL_DEBUG
    LOG.print(F("Inbound UDP packet: "));
    LOG.println(inputBuffer);
    #endif

    if (Udp.remoteIP() == WiFi.localIP())                   // не реагировать на свои же пакеты
    {
      return;
    }

    char reply[MAX_UDP_BUFFER_SIZE];
    reply [0] = '\0';
    processInputBuffer(inputBuffer, reply, true, sizeof(reply));

    #if USE_MQTT                                          // отправка ответа выполнения команд по MQTT, если разрешено
    if (espMode == 1U)
    {
      strlcpy(MqttManager::mqttBuffer, reply, sizeof(MqttManager::mqttBuffer)); // ограничиваем ответ размером MQTT-буфера
    }
    #endif
    
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.print(reply);
    Udp.endPacket();

    #if GENERAL_DEBUG
    LOG.print(F("Outbound UDP packet: "));
    LOG.println(reply);
    LOG.println();
    #endif
  }
}

void updateSets()
{
      loadingFlag = true;
      //settChanged = true;
      //eepromTimeout = millis();

      #if USE_MQTT
      if (espMode == 1U)
      {
        MqttManager::needToPublish = true;
      }
      #endif
}

void processInputBuffer(char *inputBuffer, char *outputBuffer, bool generateOutput, size_t outputBufferSize)
{
    char buff[MAX_UDP_BUFFER_SIZE], *endToken = NULL;
    String BUFF = String(inputBuffer);

    if (!strncmp_P(inputBuffer, PSTR("GET"), 3))
    {
      #ifdef GET_TIME_FROM_PHONE
      if ((!timeSynched || !(ntpServerAddressResolved && espMode == 1U)) && ((manualTimeShift + millis() / 1000UL) > (phoneTimeLastSync + GET_TIME_FROM_PHONE * 60U)))
      {// если прошло более 5 минут (GET_TIME_FROM_PHONE 5U), значит, можно парсить время из строки GET
        if (BUFF.length() > 7U){ // пускай будет хотя бы 7
          strlcpy(buff, &inputBuffer[4], sizeof(buff));          // безопасно взять подстроку с 5-го символа
          phoneTimeLastSync = (time_t)atoi(buff);
          manualTimeShift = phoneTimeLastSync - millis() / 1000UL;
          #ifdef WARNING_IF_NO_TIME
            noTimeClear();
          #endif // WARNING_IF_NO_TIME  
          timeSynched = true;
          #if defined(PHONE_N_MANUAL_TIME_PRIORITY) && defined(USE_NTP) && !USE_RTC
            stillUseNTP = false;
          #endif
          getBrightnessForPrintTime();
        }
      }
      #endif // GET_TIME_FROM_PHONE
      if (inputBuffer[3] == '-') sendCurrent(inputBuffer);
      else NEWsendCurrent(inputBuffer);
    }
    else if (!strncmp_P(inputBuffer, PSTR("EFF"), 3)){
        uint8_t temp;
        uint8_t tmp = 0;
        if((!strncmp_P(inputBuffer, PSTR("EFF_N"), 5)) || (!strncmp_P(inputBuffer, PSTR("EFF_P"), 5))){
          if (!strncmp_P(inputBuffer, PSTR("EFF_N"), 5)) tmp = 1;
          temp = jsonReadtoInt (configSetup, "eff_sel");
          if (tmp){
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
            if (random_on && FavoritesManager::FavoritesRunning)
            {
                selectedSettings = 1U;
                applyPendingRandomEffectSettings();
            }
            sendCurrent(inputBuffer);
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
    else
    {
      strlcpy(buff, &inputBuffer[3], sizeof(buff));          // безопасно взять подстроку с 4-го символа
      const int requestedMode = atoi(buff);
      if (requestedMode < 0 || requestedMode >= MODE_AMOUNT) {
        if (generateOutput && outputBuffer && outputBufferSize)
          strlcpy(outputBuffer, "ERR EFFECT", outputBufferSize);
        return;
      }
      temp = (uint8_t)requestedMode;
      currentMode = temp;
      updateSets();
    jsonWrite(configSetup, "eff_sel", temp);
    jsonWrite(configSetup, "br", modes[currentMode].Brightness);
    jsonWrite(configSetup, "sp", modes[currentMode].Speed);
    jsonWrite(configSetup, "sc", modes[currentMode].Scale);
      #if USE_MULTIPLE_LAMPS_CONTROL
      repeat_multiple_lamp_control = true;
      #endif  //USE_MULTIPLE_LAMPS_CONTROL
      sendCurrent(inputBuffer);

      #if USE_BLYNK_PLUS
      updateRemoteBlynkParams();
      #endif

        if (random_on && FavoritesManager::FavoritesRunning)
        {
          selectedSettings = 1U;
          applyPendingRandomEffectSettings();
        }
      
      SetBrightness(modes[currentMode].Brightness);
    }
    }

    #if USE_MP3_PLAYER
    else if (!strncmp_P(inputBuffer, PSTR("VOL"), 3))
    {
      strlcpy(buff, &inputBuffer[3], sizeof(buff));          // безопасно взять подстроку с 4-го символа
      uint8_t eff_sound_on_tmp = (uint8_t)atoi(buff);
      if (eff_sound_on_tmp)  {
          eff_sound_on = eff_volume = constrain( eff_sound_on_tmp, 1,30 );
          }
      else
          if (!eff_sound_on) {
              eff_sound_on = eff_volume;
              }
          else {
              eff_sound_on = 0;
              }
      
      send_command(6,0,0,eff_volume);
      jsonWrite(configSetup, "vol", eff_volume);
      jsonWrite(configSetup, "on_sound", eff_sound_on > 0 ? 1 : 0);
      
      /*
      for (uint8_t i = 0; i < 8; i++)
      {
        mp3.write(cmdbuf[i]);
        delay(3);
      }
      */
      sendVolume(inputBuffer);

    #if USE_MQTT
      if (espMode == 1U)
      {
        MqttManager::needToPublish = true;
      }
    #endif

    #if USE_BLYNK_PLUS
        updateRemoteBlynkParams();
    #endif
    #if USE_MULTIPLE_LAMPS_CONTROL
        repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
    }
    
    else if (!strncmp_P(inputBuffer, PSTR("SO_ON"), 5))
    {
      eff_sound_on = eff_volume;
      jsonWrite(configSetup, "on_sound", 1);
      timeout_save_file_changes = millis();
      bitSet (save_file_changes, 0);
      sendVolume(inputBuffer);

  #if USE_MQTT
      if (espMode == 1U)
      {
        MqttManager::needToPublish = true;
      }
  #endif
  #if USE_BLYNK_PLUS
      updateRemoteBlynkParams();
  #endif
    #if USE_MULTIPLE_LAMPS_CONTROL
        repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
    }

    else if (!strncmp_P(inputBuffer, PSTR("SO_OFF"), 6))
    {
      eff_sound_on = 0;
      jsonWrite(configSetup, "on_sound", 0);
      timeout_save_file_changes = millis();
      bitSet (save_file_changes, 0);
      sendVolume(inputBuffer);

  #if USE_MQTT
      if (espMode == 1U)
      {
        MqttManager::needToPublish = true;
      }
  #endif
  #if USE_BLYNK_PLUS
      updateRemoteBlynkParams();
  #endif
    #if USE_MULTIPLE_LAMPS_CONTROL
        repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
    }    
    #endif  // USE_MP3_PLAYER

    else if (!strncmp_P(inputBuffer, PSTR("BRI"), 3))
    {
      strlcpy(buff, &inputBuffer[3], sizeof(buff));          // безопасно взять подстроку с 4-го символа
      modes[currentMode].Brightness = constrain(atoi(buff), 1, EFFECT_BRIGHTNESS_MAX);
      jsonWrite(configSetup, "br", modes[currentMode].Brightness);
      markEffectSettingsChanged();
      #if USE_MULTIPLE_LAMPS_CONTROL
      repeat_multiple_lamp_control = true;
      #endif  //USE_MULTIPLE_LAMPS_CONTROL
      SetBrightness(modes[currentMode].Brightness);
      //loadingFlag = true; //не хорошо делать перезапуск эффекта после изменения яркости, но в некоторых эффектах от чётности яркости мог бы зависеть внешний вид
      //settChanged = true;
      //eepromTimeout = millis();
      sendCurrent(inputBuffer);

      #if USE_MQTT
      if (espMode == 1U)
      {
        MqttManager::needToPublish = true;
      }
      #endif

      #if USE_BLYNK_PLUS
      updateRemoteBlynkParams();
      #endif
    }

    else if (!strncmp_P(inputBuffer, PSTR("SPD"), 3))
    {
      strlcpy(buff, &inputBuffer[3], sizeof(buff));          // безопасно взять подстроку с 4-го символа
      modes[currentMode].Speed = constrain(atoi(buff), 1, 255);
      jsonWrite(configSetup, "sp", modes[currentMode].Speed);
      markEffectSettingsChanged();
      #if USE_BLYNK_PLUS
      updateRemoteBlynkParams();
      #endif
      #if USE_MULTIPLE_LAMPS_CONTROL
      repeat_multiple_lamp_control = true;
      #endif  //USE_MULTIPLE_LAMPS_CONTROL
      updateSets();
      sendCurrent(inputBuffer);
    }

    else if (!strncmp_P(inputBuffer, PSTR("SCA"), 3))
    {
      strlcpy(buff, &inputBuffer[3], sizeof(buff));          // безопасно взять подстроку с 4-го символа
      modes[currentMode].Scale = constrain(atoi(buff), 1, 255);
      jsonWrite(configSetup, "sc", modes[currentMode].Scale);
      markEffectSettingsChanged();
      #if USE_MULTIPLE_LAMPS_CONTROL
      repeat_multiple_lamp_control = true;
      #endif  //USE_MULTIPLE_LAMPS_CONTROL
      updateSets();
      sendCurrent(inputBuffer);
      #if USE_BLYNK_PLUS
      updateRemoteBlynkParams();
      #endif
    }

    else if (!strncmp_P(inputBuffer, PSTR("P_ON"), 4))
    {
      if (dawnFlag == 1) {
        manualOff = true;
        dawnFlag = 2;
        #if USE_TM1637
        clockTicker_blink();
        #endif
        SetBrightness(modes[currentMode].Brightness);
        changePower();
        sendCurrent(inputBuffer);
      }
      else if (sunsetFlag == 1) {
        manualsOff = true;
        sunsetFlag = 2;
        #if USE_TM1637
        clockTicker_blink();
        #endif
        SetBrightness(modes[currentMode].Brightness);
        changePower();
        sendCurrent(inputBuffer);
      }
      else {
        ONflag = true;
        jsonWrite(configSetup, "Power", ONflag);
        restoreEffectSettingsForPowerOn();
        timeout_save_file_changes = millis();
        bitSet (save_file_changes, 0);
        updateSets();
        changePower();
        #if USE_MULTIPLE_LAMPS_CONTROL
        repeat_multiple_lamp_control=true;
        #endif  //USE_MULTIPLE_LAMPS_CONTROL
        sendCurrent(inputBuffer);
        #if USE_BLYNK_PLUS
        updateRemoteBlynkParams();
        #endif
      }  
    }

    else if (!strncmp_P(inputBuffer, PSTR("P_OFF"), 5))
    {
      if (dawnFlag == 1) {
        manualOff = true;
        dawnFlag = 2;
        #if USE_TM1637
        clockTicker_blink();
        #endif
        SetBrightness(modes[currentMode].Brightness);
        saveEffectSettingsNow(false);
        changePower();
        sendCurrent(inputBuffer);
      }
      else if (sunsetFlag == 1) {
        manualsOff = true;
        sunsetFlag = 2;
        #if USE_TM1637
        clockTicker_blink();
        #endif
        SetBrightness(modes[currentMode].Brightness);
        saveEffectSettingsNow(false);
        changePower();
        sendCurrent(inputBuffer);
      }
      else {
        ONflag = false;
        jsonWrite(configSetup, "Power", ONflag);
        persistEffectSettingsBeforePowerOff();
        changePower();
        #if USE_MULTIPLE_LAMPS_CONTROL
        multiple_lamp_control ();
        #endif  //USE_MULTIPLE_LAMPS_CONTROL
        sendCurrent(inputBuffer);

        #if USE_MQTT
        if (espMode == 1U)
        {
          MqttManager::needToPublish = true;
        }
        #endif
        #if USE_BLYNK_PLUS
        updateRemoteBlynkParams();
        #endif
      }
    }

#if USE_MULTIPLE_LAMPS_CONTROL
    else if (!strncmp_P(inputBuffer, PSTR("MULTI"), 5)) { // Управление несколькими лампами
      uint8_t valid = 0;
      size_t i = 0U;
      bool emptyField = false;
      while (inputBuffer[i])   {   //пакет должен иметь вид MULTI,%U,%U,%U,%U,%U соответственно ON/OFF,№эффекта,яркость,скорость,масштаб или + №текущей папки или + озвучування_on/off, гучнисть
        if (inputBuffer[i] == ',')  {
          valid++;
          if (inputBuffer[i + 1U] == ',' || inputBuffer[i + 1U] == '\0')
            emptyField = true;
        } //Проверка на правильность пакета (по количеству запятых)
        i++;
      }
      if (!emptyField && (valid == 5 || valid == 6 || valid == 8))   {   //Если пакет правильный выделяем лексемы,разделённые запятыми, и присваиваем параметрам эффектов
        char *tmp = strtok (inputBuffer, ","); //Первая лексема MULTI пропускается
        tmp = strtok (NULL, ",");
        bool onflg = false;
        bool effectSettingsChanged = false;
        if (ONflag != atoi(tmp))   {
        ONflag = atoi(tmp) != 0;
        onflg = true;
        if (ONflag) restoreEffectSettingsForPowerOn();
        //changePower();   // Активация состояния ON/OFF
        }
        tmp = strtok (NULL, ",");
        const int incomingMode = atoi(tmp);
        if (currentMode != incomingMode)   {
          if (incomingMode >= 0 && incomingMode < MODE_AMOUNT)   {
          currentMode = (uint8_t)incomingMode;
          tmp = strtok (NULL, ",");
        const uint8_t incomingBrightness = constrain(atoi(tmp), 1, EFFECT_BRIGHTNESS_MAX);
          tmp = strtok (NULL, ",");
        const uint8_t incomingSpeed = constrain(atoi(tmp), 1, 255);
          tmp = strtok (NULL, ",");
        const uint8_t incomingScale = constrain(atoi(tmp), 1, 255);
        effectSettingsChanged =
            modes[currentMode].Brightness != incomingBrightness ||
            modes[currentMode].Speed != incomingSpeed ||
            modes[currentMode].Scale != incomingScale;
        modes[currentMode].Brightness = incomingBrightness;
        modes[currentMode].Speed = incomingSpeed;
        modes[currentMode].Scale = incomingScale;
          #if USE_MP3_PLAYER
          if (valid == 8) {
          tmp = strtok (NULL, ",");
        eff_sound_on = atoi (tmp);
          tmp = strtok (NULL, ",");
        eff_volume = atoi (tmp);
          if (!dawnFlag && ONflag && eff_sound_on) {
            send_command(6,FEEDBACK,0,eff_volume); // Меняем громкость
            delay(mp3_delay);
          }
          if (!sunsetFlag && ONflag && eff_sound_on) {
            send_command(6,FEEDBACK,0,eff_volume); // Меняем громкость
            delay(mp3_delay);
          }
          }
          if (valid == 8 || valid == 6) {
            tmp = strtok (NULL, ",");
            mp3_folder=effects_folders[currentMode];
            mp3_folder_last = mp3_folder;
            if (atoi (tmp) != CurrentFolder) {
              CurrentFolder = atoi (tmp);
              if (!dawnFlag && ONflag && eff_sound_on) {
                send_command(0x17,FEEDBACK,0,CurrentFolder); // Включить непрерывное воспроизведение указанной папки
                //mp3_folder_change = 0;
                CurrentFolder_last = CurrentFolder;
                mp3_stop = false;
                delay(mp3_delay);
              }
              if (!sunsetFlag && ONflag && eff_sound_on) {
                send_command(0x17,FEEDBACK,0,CurrentFolder); // Включить непрерывное воспроизведение указанной папки
                //mp3_folder_change = 0;
                CurrentFolder_last = CurrentFolder;
                mp3_stop = false;
                delay(mp3_delay);
              }
            }
          }
          #endif // USE_MP3_PLAYER
          loadingFlag = true; // Перезапуск эффекта
          SetBrightness(modes[currentMode].Brightness); //Применение яркости
          }
          else   {
            currentMode = MODE_AMOUNT - 3;  //Если полученный номер эффекта больше , чем количество эффектов в лампе,включаем последний "адекватный" эффект
            loadingFlag = true; // Перезапуск эффекта
            SetBrightness(modes[currentMode].Brightness); //Применение яркости
          }
        }
        else   {
            tmp = strtok (NULL, ",");
            const uint8_t incomingBrightness = constrain(atoi(tmp), 1, EFFECT_BRIGHTNESS_MAX);
            if (modes[currentMode].Brightness != incomingBrightness)   {
                modes[currentMode].Brightness = incomingBrightness;
                effectSettingsChanged = true;
                SetBrightness(modes[currentMode].Brightness); //Применение яркости
            }
            tmp = strtok (NULL, ",");
            const uint8_t incomingSpeed = constrain(atoi(tmp), 1, 255);
            if (modes[currentMode].Speed != incomingSpeed)   {
                modes[currentMode].Speed = incomingSpeed;
                effectSettingsChanged = true;
                loadingFlag = true; // Перезапуск эффекта
            }
            tmp = strtok (NULL, ",");
            const uint8_t incomingScale = constrain(atoi(tmp), 1, 255);
                if (modes[currentMode].Scale != incomingScale)   {
                modes[currentMode].Scale = incomingScale;
                effectSettingsChanged = true;
                loadingFlag = true; // Перезапуск эффекта
            }
          #if USE_MP3_PLAYER
          if (valid == 8) {
          tmp = strtok (NULL, ",");
        eff_sound_on = atoi (tmp);
          tmp = strtok (NULL, ",");
        eff_volume = atoi (tmp);
          if (!dawnFlag && ONflag && eff_sound_on) {
            send_command(6,FEEDBACK,0,eff_volume); // Меняем громкость
            delay(mp3_delay);
          }
          if (!sunsetFlag && ONflag && eff_sound_on) {
            send_command(6,FEEDBACK,0,eff_volume); // Меняем громкость
            delay(mp3_delay);
          }
          }
          if (valid == 8 || valid == 6) {
            tmp = strtok (NULL, ",");
            if (atoi (tmp) != CurrentFolder) {
              CurrentFolder = atoi (tmp);
              if (eff_sound_on && !dawnFlag && ONflag) {
                send_command(0x17,FEEDBACK,0,CurrentFolder); // Включить непрерывное воспроизведение указанной папки
                mp3_stop = false;
                //mp3_folder_change = 0;
                CurrentFolder_last = CurrentFolder;
                delay(mp3_delay);
                //send_sound_flag = 1;
              }
              if (eff_sound_on && !sunsetFlag && ONflag) {
                send_command(0x17,FEEDBACK,0,CurrentFolder); // Включить непрерывное воспроизведение указанной папки
                mp3_stop = false;
                //mp3_folder_change = 0;
                CurrentFolder_last = CurrentFolder;
                delay(mp3_delay);
                //send_sound_flag = 1;
              }
            }
          }
          #endif // USE_MP3_PLAYER
        }
        if (effectSettingsChanged) markEffectSettingsChanged();
        if (onflg) {
            #if USE_MP3_PLAYER
            if (ONflag) mp3_folder=effects_folders[currentMode];
            #endif
            if (!ONflag) persistEffectSettingsBeforePowerOff();
            changePower();   // Активация состояния ON/OFF
        }
 #if GENERAL_DEBUG
     LOG.print ("Принято MULTI ");
     LOG.println (ONflag);
     LOG.println (currentMode);
     LOG.println (modes[currentMode].Brightness);
     LOG.println (modes[currentMode].Speed);
     LOG.println (modes[currentMode].Scale);
     #if USE_MP3_PLAYER
     LOG.println (CurrentFolder);
     LOG.println (eff_sound_on);
     LOG.println (eff_volume);
     #endif // USE_MP3_PLAYER
 #endif  //GENERAL_DEBUG
     //changePower();   // Активацмя состояния ON/OFF
     //loadingFlag = true; // Перезапуск эффекта
     //FastLED.setBrightness(modes[currentMode].Brightness); //Применение яркости
     jsonWrite(configSetup, "br", modes[currentMode].Brightness); //Передаём в веб интерфейс новые параметры 
     jsonWrite(configSetup, "sp", modes[currentMode].Speed);      //для правильного отображения
     jsonWrite(configSetup, "sc", modes[currentMode].Scale);
     jsonWrite(configSetup, "eff_sel", currentMode);
     #if USE_MP3_PLAYER
     jsonWrite(configSetup, "on_sound", eff_sound_on > 0 ? 1 : 0);
     jsonWrite(configSetup, "vol", eff_volume);
     jsonWrite(configSetup, "fold_sel", CurrentFolder);
     #endif // USE_MP3_PLAYER

     jsonWrite(configSetup, "Power", ONflag);
     }
     inputBuffer[0] = '\0';
     //outputBuffer[0] = '\0';
     generateOutput = false;
    }
#endif //USE_MULTIPLE_LAMPS_CONTROL

    else if (!strncmp_P(inputBuffer, PSTR("FAV_"), 4)) { // сокращаем GET и SET для ускорения регулярного цикла
      if (!strncmp_P(inputBuffer, PSTR("FAV_ON"), 6))
       {
         if (ONflag)   {
         FavoritesManager::FavoritesRunning = 1;
         jsonWrite(configSetup, "cycle_on", 1);
        }
        else {
             FavoritesManager::FavoritesRunning = 0;
             FavoritesManager::nextModeAt = 0;
             jsonWrite(configSetup, "cycle_on", 0);
        } 
      }
      else if(!strncmp_P(inputBuffer, PSTR("FAV_OFF"), 7))
      {
        FavoritesManager::FavoritesRunning = 0;
        FavoritesManager::nextModeAt = 0;
        jsonWrite(configSetup, "cycle_on", 0);
      }
      else 
      {
      if (!strncmp_P(inputBuffer, PSTR("FAV_SET"), 7))
      {
        FavoritesManager::ConfigureFavorites(inputBuffer);
        if (!ONflag) FavoritesManager::FavoritesRunning = 0;
        FavoritesManager::SetStatus(inputBuffer);
        jsonWrite(configSetup, "cycle_on", FavoritesManager::FavoritesRunning);  // чтение состояния настроек режима Цикл 
        jsonWrite(configSetup, "time_eff", FavoritesManager::Interval);          // вкл/выкл,время переключения,дисперсия,вкл цикла после перезагрузки
        jsonWrite(configSetup, "disp", FavoritesManager::Dispersion);
        jsonWrite(configSetup, "cycle_allwase", FavoritesManager::UseSavedFavoritesRunning);
        //cycle_get();  // запмсь выбранных эффектов
        timeout_save_file_changes = millis();
        bitSet (save_file_changes, 2);
    
        #if USE_MQTT
        if (espMode == 1U)
        {
          MqttManager::needToPublish = true;
        }
        #endif
      }
      else if (!strncmp_P(inputBuffer, PSTR("FAV_GET"), 7))
        FavoritesManager::SetStatus(inputBuffer);
      }
    }

    #if USE_OTA
    else if (!strncmp_P(inputBuffer, PSTR("OTA"), 3))
    {
      //if (espMode == 1U) пускай обновление работает даже в режиме точки доступа
      // && otaManager.RequestOtaUpdate()){ по идее, нужен положительный ответ от менеджера
        otaManager.RequestOtaUpdate(); // но из-за двойного запроса нихрена не работает
        delay(70);
        //if (otaManager.RequestOtaUpdate()) //по идее, нужен положительный ответ от менеджера
        otaManager.RequestOtaUpdate(); // но если уже был один ответ из двух в прошлый раз, то сейчас второй лучше не проверять
        if (OtaManager::OtaFlag == OtaPhase::InProgress) {
          if (!ONflag) restoreEffectSettingsForPowerOn();
          currentMode = EFF_MATRIX;                             // принудительное включение режима "Матрица" для индикации перехода в режим обновления по воздуху
          FastLED.clear();
          delay(1);
          ONflag = true;
      jsonWrite(configSetup, "Power", ONflag);
      jsonWrite(configSetup, "eff_sel", currentMode);
      jsonWrite(configSetup, "br", modes[currentMode].Brightness);
      jsonWrite(configSetup, "sp", modes[currentMode].Speed);
      jsonWrite(configSetup, "sc", modes[currentMode].Scale);
          changePower();
        }
        else
          showWarning(CRGB::Red, 2000U, 500U);                     // мигание красным цветом 2 секунды (ошибка)
    }
    #endif // OTA

    #if USE_BUTTON
    else if (!strncmp_P(inputBuffer, PSTR("BTN"), 3))
    {
     String configHardware = readFile(F("config_hardware.json"), 2048);

    if (strstr_P(inputBuffer, PSTR("ON")) - inputBuffer == 4)
  {
    buttonEnabled = true;
    jsonWrite(configHardware, "button_on", (int)buttonEnabled);
    writeFile(F("config_hardware.json"), configHardware);
    sendCurrent(inputBuffer);
  }
  else
  {
    buttonEnabled = false;
    jsonWrite(configHardware, "button_on", (int)buttonEnabled);
    writeFile(F("config_hardware.json"), configHardware);
    sendCurrent(inputBuffer);
  }

      #if USE_MQTT
      if (espMode == 1U)
      {
        strlcpy(MqttManager::mqttBuffer, inputBuffer, sizeof(MqttManager::mqttBuffer));
        MqttManager::needToPublish = true;
      }
      #endif
    }
    #endif //USE_BUTTON
    
    else if (!strncmp_P(inputBuffer, PSTR("GBR"), 3)) // выставляем общую яркость для всех эффектов без сохранения в EEPROM, если приложение присылает такую строку
    {
      strlcpy(buff, &inputBuffer[3], sizeof(buff));          // безопасно взять подстроку с 4-го символа
      uint8_t ALLbri = constrain(atoi(buff), 1, EFFECT_BRIGHTNESS_MAX);
      for (uint8_t i = 0; i < MODE_AMOUNT; i++) {
        modes[i].Brightness = ALLbri;
      }
      markEffectSettingsChanged();
    jsonWrite(configSetup, "br", ALLbri);
      SetBrightness(ALLbri);
      loadingFlag = true;
      #if USE_MULTIPLE_LAMPS_CONTROL
      repeat_multiple_lamp_control = true;
      #endif  //USE_MULTIPLE_LAMPS_CONTROL
    }
    
    #ifdef USE_RANDOM_SETS_IN_APP
    else if (!strncmp_P(inputBuffer, PSTR("RND_"), 4)) // управление включением случайных настроек
    {
       if (!strncmp_P(inputBuffer, PSTR("RND_0"), 5)) // вернуть настройки по умолчанию текущему эффекту
       {
         if (resetCurrentEffectToDefaults()) {
           SetBrightness(modes[currentMode].Brightness);
           updateSets();
           #if USE_MULTIPLE_LAMPS_CONTROL
           repeat_multiple_lamp_control = true;
           #endif  //USE_MULTIPLE_LAMPS_CONTROL
         }
         sendCurrent(inputBuffer);
       }
       else if (!strncmp_P(inputBuffer, PSTR("RND_1"), 5)) // выбрать случайные настройки текущему эффекту
       { // раньше была идея, что будут числа RND_1, RND_2, RND_3 - выбор из предустановленных настроек, но потом всё свелось к единственному варианту случайных настроек
         selectedSettings = 1U;
         updateSets();
         applyPendingRandomEffectSettings();
         #if USE_MULTIPLE_LAMPS_CONTROL
         repeat_multiple_lamp_control = true;
         #endif  //USE_MULTIPLE_LAMPS_CONTROL
       }
       else if (!strncmp_P(inputBuffer, PSTR("RND_Z"), 5)) // вернуть настройки по умолчанию всем эффектам
       {
         if (!resetAllEffectsToDefaults()) return;
         updateSets();
         #if USE_MULTIPLE_LAMPS_CONTROL
         repeat_multiple_lamp_control = true;
         #endif  //USE_MULTIPLE_LAMPS_CONTROL
         sendCurrent(inputBuffer);
         #if USE_BLYNK
         updateRemoteBlynkParams();
         #endif
       }
       else if (!strncmp_P(inputBuffer, PSTR("RND_C1"), 5)) // Включаем случайный выбор эффектов в цикле
       {
        FavoritesManager::rndCycle = 1;
      jsonWrite(configSetup, "rnd_cycle", 1);
        saveConfig();
       }
       else if (!strncmp_P(inputBuffer, PSTR("RND_C0"), 5)) // Выключаем случайный выбор эффектов в цикле
       {
        FavoritesManager::rndCycle = 0;
      jsonWrite(configSetup, "rnd_cycle", 0);
        saveConfig();
       }
       else if (!strncmp_P(inputBuffer, PSTR("RND_ON"), 6)) // включить выбор случайных настроек в режиме Цикл
       {
         random_on = 1U;
    jsonWrite(configSetup, "random_on", (int)random_on);
    saveConfig();  
         showWarning(CRGB::Blue, 1000U, 500U);                    // мигание синим цветом 1 секунду
       }
       else if (!strncmp_P(inputBuffer, PSTR("RND_OFF"), 7)) // отключить выбор случайных настроек в режиме Цикл
       {
         random_on = 0U;
    jsonWrite(configSetup, "random_on", (int)random_on);
    saveConfig();  
         showWarning(CRGB::Blue, 1000U, 500U);                    // мигание синим цветом 1 секунду
       }
    }
    #endif //#ifdef USE_RANDOM_SETS_IN_APP
    
    else if (!strncmp_P(inputBuffer, PSTR("RUN_"), 4)) {          // Настройка бегущей строки
        if (!strncmp_P(inputBuffer, PSTR("RUN_T"), 5))            // Периодичность вывода (минуты). 0-не выводить; больше 60-выводить постоянно
        {
            strlcpy(buff, &inputBuffer[5], sizeof(buff));          // безопасно взять подстроку с 6-го символа
            RuninTextOverEffects = (uint8_t)atoi(buff);
            jsonWrite(configSetup, "toe", RuninTextOverEffects);
            bitSet (save_file_changes, 0);
            timeout_save_file_changes = millis();
        }
        else if (!strncmp_P(inputBuffer, PSTR("RUN_C"), 5))       // Цвет бегущей строки (0-255)
        {
            strlcpy(buff, &inputBuffer[5], sizeof(buff));
            ColorRunningText = (uint8_t)atoi(buff);
            jsonWrite(configSetup, "sct", ColorRunningText);
            bitSet (save_file_changes, 0);
            timeout_save_file_changes = millis();
        }
        else if (!strncmp_P(inputBuffer, PSTR("RUN_F"), 5))       // фон бегущей строки 0-черный фон; 1-цветный фон
        {
            strlcpy(buff, &inputBuffer[5], sizeof(buff));
            ColorTextFon = (uint8_t)atoi(buff);
            jsonWrite(configSetup, "ctf", ColorTextFon);
            bitSet (save_file_changes, 0);
            timeout_save_file_changes = millis();
        }
        else if (!strncmp_P(inputBuffer, PSTR("RUN_S"), 5))       // скорость бегущей строки (0-255)
        {
            strlcpy(buff, &inputBuffer[5], sizeof(buff));
            SpeedRunningText = (uint8_t)atoi(buff);
            jsonWrite(configSetup, "spt", SpeedRunningText);
            bitSet (save_file_changes, 0);
            timeout_save_file_changes = millis();
        }
    }
    
    #ifdef BUTTON_CAN_SET_SLEEP_TIMER
    else if (!strncmp_P(inputBuffer, PSTR("SLEEP"), 5)) { // Таймер сна. SLEEP2 – 10 минут. Иначе – 5 минут
    strlcpy(buff, &inputBuffer[5], sizeof(buff));
    uint8_t temp = (uint8_t)atoi(buff);
    showWarning(CRGB::Blue, 1000, 250U);     // Мигание синим цветом 1 секунду. Мигать об успехе операции лучше до вызова changePower(), иначе сперва мелькнут кадры текущего эффекта
    if (!ONflag) restoreEffectSettingsForPowerOn();
    ONflag = true;
    changePower();
    jsonWrite(configSetup, "Power", ONflag);
    jsonWrite(configSetup, "tmr", 1);
    #if USE_BLYNK
    updateRemoteBlynkParams();
    #endif
    if (temp == 2) TimerManager::TimeToFire = millis() + BUTTON_SET_SLEEP_TIMER2 * 60UL * 1000UL;
    else TimerManager::TimeToFire = millis() + BUTTON_SET_SLEEP_TIMER1 * 60UL * 1000UL;
    TimerManager::TimerRunning = true;
    }
    #endif //BUTTON_CAN_SET_SLEEP_TIMER 

    else if (!strncmp_P(inputBuffer, PSTR("ALM_"), 4)) { // сокращаем GET и SET для ускорения регулярного цикла
      if (!strncmp_P(inputBuffer, PSTR("ALM_SET"), 7))
      {
        uint8_t alarmNum = (char)inputBuffer[7] - '0';
        alarmNum -= 1;
        if (strstr_P(inputBuffer, PSTR("ON")) - inputBuffer == 9)
        {
          alarms[alarmNum].State = true;
          sendAlarms(inputBuffer);
        }
        else if (strstr_P(inputBuffer, PSTR("OFF")) - inputBuffer == 9)
        {
          alarms[alarmNum].State = false;
          sendAlarms(inputBuffer);
        }
        else
        {
          strlcpy(buff, &inputBuffer[8], sizeof(buff));        // безопасно взять подстроку с 9-го символа
          alarms[alarmNum].Time = atoi(buff);
          sendAlarms(inputBuffer);
        }
        //EepromManager::SaveAlarmsSettings(&alarmNum, alarms);

        #if USE_MQTT
        if (espMode == 1U)
        {
          strlcpy(MqttManager::mqttBuffer, inputBuffer, sizeof(MqttManager::mqttBuffer));
          MqttManager::needToPublish = true;
        }
        #endif
      }
      else
        sendAlarms(inputBuffer);
    }

    else if (!strncmp_P(inputBuffer, PSTR("SUN_"), 4)) { // сокращаем GET и SET для ускорения регулярного цикла
      if (!strncmp_P(inputBuffer, PSTR("SUN_SET"), 7))
      {
        uint8_t sunsetNum = (char)inputBuffer[7] - '0';
        sunsetNum -= 1;
        if (strstr_P(inputBuffer, PSTR("ON")) - inputBuffer == 9)
        {
          sunsets[sunsetNum].State = true;
          sendSunsets(inputBuffer);
        }
        else if (strstr_P(inputBuffer, PSTR("OFF")) - inputBuffer == 9)
        {
          sunsets[sunsetNum].State = false;
          sendSunsets(inputBuffer);
        }
        else
        {
          strlcpy(buff, &inputBuffer[8], sizeof(buff));        // безопасно взять подстроку с 9-го символа
          sunsets[sunsetNum].Time = atoi(buff);
          sendSunsets(inputBuffer);
        }
        //EepromManager::SaveAlarmsSettings(&alarmNum, alarms);

        #if USE_MQTT
        if (espMode == 1U)
        {
          strlcpy(MqttManager::mqttBuffer, inputBuffer, sizeof(MqttManager::mqttBuffer));
          MqttManager::needToPublish = true;
        }
        #endif
      }
      else
        sendSunsets(inputBuffer);
    }


    else if (!strncmp_P(inputBuffer, PSTR("DAWN"), 4))
    {
      strlcpy(buff, &inputBuffer[4], sizeof(buff));          // безопасно взять подстроку с 5-го символа
      dawnMode = atoi(buff) - 1;
      //EepromManager::SaveDawnMode(&dawnMode);
      sendAlarms(inputBuffer);

      #if USE_MQTT
      if (espMode == 1U)
      {
        MqttManager::needToPublish = true;
      }
      #endif
    }

    else if (!strncmp_P(inputBuffer, PSTR("SUNS"), 4))
    {
      strlcpy(buff, &inputBuffer[4], sizeof(buff));          // безопасно взять подстроку с 5-го символа
      sunsetMode = atoi(buff) - 1;
      //EepromManager::SaveDawnMode(&dawnMode);
      sendSunsets(inputBuffer);

      #if USE_MQTT
      if (espMode == 1U)
      {
        MqttManager::needToPublish = true;
      }
      #endif
    }
    
    else if (!strncmp_P(inputBuffer, PSTR("DISCOVER"), 8))  // обнаружение приложением модуля esp в локальной сети
    {
      // В AP fallback espMode остаётся равен 1, а WiFi.localIP() равен
      // 0.0.0.0. Сообщаем фактический адрес активного интерфейса.
      IPAddress activeIp = getActiveLampIP();
      char lamp_name[LAMP_NAME.length() + 1];
      LAMP_NAME.toCharArray(lamp_name, LAMP_NAME.length() + 1);
      snprintf_P(inputBuffer, sizeof(inputBuffer), PSTR("IP %u.%u.%u.%u:%u:%s"),
        activeIp[0], activeIp[1], activeIp[2], activeIp[3],
        ESP_UDP_PORT, lamp_name);
    }

    else if (!strncmp_P(inputBuffer, PSTR("TMR_"), 4)) { // сокращаем GET и SET для ускорения регулярного цикла
      if (!strncmp_P(inputBuffer, PSTR("TMR_SET"), 7))
      {
        if (strlen(inputBuffer) < 13U) {
          inputBuffer[0] = '\0';
          return;
        }
        memcpy(buff, &inputBuffer[8], 2);                     // взять подстроку, состоящую из 9 и 10 символов, из строки inputBuffer
        buff[2] = '\0';
        TimerManager::TimerRunning = (bool)atoi(buff);

        memcpy(buff, &inputBuffer[10], 2);                    // взять подстроку, состоящую из 11 и 12 символов, из строки inputBuffer
        buff[2] = '\0';
        TimerManager::TimerOption = (uint8_t)atoi(buff);

        strlcpy(buff, &inputBuffer[12], sizeof(buff));         // безопасно взять подстроку с 13-го символа
        TimerManager::TimeToFire = millis() + strtoull(buff, &endToken, 10) * 1000;

        TimerManager::TimerHasFired = false;
        sendTimer(inputBuffer);

        #if USE_MQTT
        if (espMode == 1U)
        {
          MqttManager::needToPublish = true;
        }
        #endif
      }
      else
        sendTimer(inputBuffer);
    }

    else if (!strncmp_P(inputBuffer, PSTR("LIST"), 4)) // передача списка эффектов по запросу от приложения (если поддерживается приложением)
    {
       strlcpy(buff, &inputBuffer[4], sizeof(buff));         // безопасно взять подстроку с 5-го символа
       switch (atoi(buff))
         {
           case 1U:
           {
             EffectList (F("/efflist1"));
             break;
           }
           case 2U:
           {
             EffectList (F("/efflist2"));
             break;
           }
           case 3U:
           {
             EffectList (F("/efflist3"));
             break;
           }
         }
    }
    
    else if (!strncmp_P(inputBuffer, PSTR("ssid"), 4)){        // Сохрание SSID для подключения к WiFi роутера
      jsonWrite(configSetup, "ssid", BUFF.substring(5, BUFF.length()));        
      saveConfig();            
    }
    
    else if (!strncmp_P(inputBuffer, PSTR("passw"), 5)){        // Сохрание пароля для подключения к WiFi роутера 
      jsonWrite(configSetup, "password", BUFF.substring(6, BUFF.length()));        
      saveConfig();            
    }          
    else if (!strncmp_P(inputBuffer, PSTR("timeout"), 7)){     // Сохрание таймаута - времени попытки подключения к WiFi роутера
      jsonWrite(configSetup, "TimeOut", BUFF.substring(8, BUFF.length()));        
      saveConfig();            
    }
    
    else if (!strncmp_P(inputBuffer, PSTR("esp_mode=0"), 10)){  // Изменение режима на точку доступа
    espMode = 0U;
    jsonWrite(configSetup, "ESP_mode", (int)espMode);
    saveConfig();  
    showWarning(CRGB::Blue, 1000U, 500U);                    // мигание синим цветом 1 секунду - смена рабочего режима лампы, перезагрузка
    ESP.restart();
  }
  
  else if (!strncmp_P(inputBuffer, PSTR("esp_mode=1"), 10)){  // Изменение режима для использования подключения к роутеру
    espMode = 1U;
    jsonWrite(configSetup, "ESP_mode", (int)espMode);
    saveConfig();  
    showWarning(CRGB::Blue, 1000U, 500U);                    // мигание синим цветом 1 секунду - смена рабочего режима лампы, перезагрузка
    ESP.restart();
  }    
  
    else if (!strncmp_P(inputBuffer, PSTR("TXT"), 3)){     // Принимаем текст для бегущей строки
        //String str = getValue(BUFF, '-', 1); // этим способом дефисы нельзя в бегущую строку передать. почему вообще разделитель - дефис?!
        String str = (BUFF.length() > 4) ? BUFF.substring(4, BUFF.length()) : "";
        str.toCharArray(TextTicker, str.length() + 1);
    }
    
    else if (!strncmp_P(inputBuffer, PSTR("DRW"), 3)) {
      drawPixelXY((int8_t)getValue(BUFF, ';', 1).toInt(), (int8_t)getValue(BUFF, ';', 2).toInt(), DriwingColor);
      FastLED.show();
    }
    
    else if (!strncmp_P(inputBuffer, PSTR("CLR"), 3)) {
      FastLED.clear();
      FastLED.show();
    }
    
    else if (!strncmp_P(inputBuffer, PSTR("COL"), 3)) {
      #ifdef USE_OLD_APP_FROM_KOTEYKA // (в версии 2.3... цвета были только в формате RGB)
       DriwingColor = CRGB(getValue(BUFF, ';', 1).toInt(), getValue(BUFF, ';', 2).toInt(), getValue(BUFF, ';', 3).toInt());
      #else
       DriwingColor = CRGB(getValue(BUFF, ';', 1).toInt(), getValue(BUFF, ';', 3).toInt(), getValue(BUFF, ';', 2).toInt());
      #endif
    }
    
    else if (!strncmp_P(inputBuffer, PSTR("DRAWO"), 5)) { // сокращаем OFF и ON для ускорения регулярного цикла
      if (!strncmp_P(inputBuffer, PSTR("DRAWON"), 6))
        Painting = 1;
      else
        Painting = 0;
        //FastLED.clear();
        //FastLED.show();
    }
    
    else if (!strncmp_P(inputBuffer, PSTR("RESET"), 5)) { // сброс настроек WIFI по запросу от приложения
      if (!strncmp_P(inputBuffer, PSTR("RESET_ALL"), 9))  // Скидання налаштувань у початкові (за замовчуванням)
      {
        LOG.println("\n*** Reset to Default ***");
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
        delay(100);
        ESP.restart();
      }
    else
      {
        jsonWrite(configSetup, "ssid", "");                         // сброс сохранённых SSID и пароля (сброс настроек подключения к роутеру)
        jsonWrite(configSetup, "password", "");
        saveConfig();                                       // Функция сохранения данных во Flash
        showWarning(CRGB::Blue, 2000U, 500U);                    // мигание синим цветом 2 секунды
      }
    }
    
#if defined(GENERAL_DEBUG) || defined(USE_OLD_IOS_APP)
    else if (!strncmp_P(inputBuffer, PSTR("DEB"), 3))
    {
        //#ifdef USE_NTP
        #if defined(USE_NTP) || defined(USE_MANUAL_TIME_SETTING) || defined(GET_TIME_FROM_PHONE)
        getFormattedTime(inputBuffer);
        sprintf_P(inputBuffer, PSTR("OK %s"), inputBuffer);
        #else
        strcpy_P(inputBuffer, PSTR("OK --:--"));
        #endif
    }
#endif
#if USE_MQTT
    else if  (!strncmp_P(inputBuffer, PSTR("STATE"), 5) && (espMode == 1U)){
        MqttManager::needToPublish = true;
    }
#endif //USE_MQTT
//#ifdef USE_OLD_APP_FROM_KOTEYKA // (в версии 2.3... были кнопки, чтобы сохранить настройки эффектов из приложения в лампу)
//и в новых тоже появились
    else if (!strncmp_P(inputBuffer, PSTR("SETS"), 4)) // передача настроек эффектов по запросу от приложения (если поддерживается приложением)
    {
      if (strlen(inputBuffer) < 5U) {
        inputBuffer[0] = '\0';
        return;
      }
      memcpy(buff, &inputBuffer[4], 1U);  // взять первую цифру, начиная с 5-го символа
      buff[1] = '\0';
      switch (atoi(buff))      
      {
        case 1U: // SET
          {
            strlcpy(buff, &inputBuffer[5], sizeof(buff));          // inputBuffer, начиная с символа 6
            const int requestedEffect = getValue(buff, ';', 0).toInt();
            if (requestedEffect < 0 || requestedEffect >= MODE_AMOUNT) return;
            const uint8_t eff = (uint8_t)requestedEffect;
            const uint8_t newBrightness = constrain(
                getValue(buff, ';', 1).toInt(), 1, EFFECT_BRIGHTNESS_MAX);
            const uint8_t newSpeed = constrain(
                getValue(buff, ';', 2).toInt(), 1, 255);
            const uint8_t newScale = constrain(
                getValue(buff, ';', 3).toInt(), 1, 255);
            if (modes[eff].Brightness != newBrightness ||
                modes[eff].Speed != newSpeed || modes[eff].Scale != newScale)
            {
              modes[eff].Brightness = newBrightness;
              modes[eff].Speed = newSpeed;
              modes[eff].Scale = newScale;
              markEffectSettingsChanged();
            }
            if (eff == currentMode) {
              syncCurrentEffectToConfig();
              updateSets();
              #if USE_BLYNK_PLUS
              updateRemoteBlynkParams();
              #endif
            }
            break;
          }
        case 2U: // READ
          {
            String OutString;
            char replyPacket[MAX_UDP_BUFFER_SIZE];
            for (uint8_t i = 0; i < MODE_AMOUNT; i++) {
              OutString = String(i) + ";" +  String(modes[i].Brightness) + ";" + String(modes[i].Speed) + ";" + String(modes[i].Scale) + "\n";
              OutString.toCharArray(replyPacket, MAX_UDP_BUFFER_SIZE);
              Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
              Udp.print(replyPacket);
              Udp.endPacket();
            }
            #if USE_MQTT
            if (espMode == 1U)
            {
                MqttManager::needToPublish = true;
            }
            #endif
            break;
          }
      }
    }
//#endif // ifdef USE_OLD_APP_FROM_KOTEYKA
    else
    {
      inputBuffer[0] = '\0';
    }

    if (strlen(inputBuffer) <= 0)
    {
      return;
    }

    if (generateOutput)                                     // если запрошен вывод ответа выполнения команд, копируем его в исходящий буфер
    {
      if (outputBufferSize > 0U) strlcpy(outputBuffer, inputBuffer, outputBufferSize);
    }
    inputBuffer[0] = '\0';                                  // очистка буфера, читобы не он не интерпретировался, как следующий входной пакет
}

void sendCurrent(char *outputBuffer)
{
    uint8_t n = currentMode;
  sprintf_P(outputBuffer, PSTR("CURR %u %u %u %u %u %u"),
    n,
    modes[currentMode].Brightness,
    modes[currentMode].Speed,
    modes[currentMode].Scale,
    ONflag,
    espMode);
  
  #ifdef USE_NTP
  strcat_P(outputBuffer, PSTR(" 1"));
  #else
  strcat_P(outputBuffer, PSTR(" 0"));
  #endif

  sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, (uint8_t)TimerManager::TimerRunning);
  #if USE_BUTTON
  sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, (uint8_t)buttonEnabled);
  #else
  sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, 0);
  #endif //USE_BUTTON

  //#ifdef USE_NTP
  #if defined(USE_NTP) || defined(USE_MANUAL_TIME_SETTING) || defined(GET_TIME_FROM_PHONE)
  char timeBuf[9];
  getFormattedTime(timeBuf);
  sprintf_P(outputBuffer, PSTR("%s %s"), outputBuffer, timeBuf);
  #else
  time_t currentTicks = millis() / 1000UL;
  sprintf_P(outputBuffer, PSTR("%s %02u:%02u:%02u"), outputBuffer, hour(currentTicks), minute(currentTicks), second(currentTicks));
  #endif
  #if USE_MP3_PLAYER
  sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, (uint8_t)eff_sound_on);
  //sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, (uint8_t)eff_volume);
  #endif  // USE_MP3_PLAYER
}

void NEWsendCurrent(char *outputBuffer)
{
    uint8_t n = currentMode;
  #if GENERAL_DEBUG
  LOG.println ("NEWsendCurrent");
  #endif
  sprintf_P(outputBuffer, PSTR("NEWCURR %u %u %u %u %u %u %u %u %u %u %u %u"),
    n,
    modes[currentMode].Brightness,
    modes[currentMode].Speed,
    modes[currentMode].Scale,
    ONflag,
    espMode,
  #ifdef USE_NTP
  1,
  #else
  0,
  #endif
    TimerManager::TimerRunning,
    #if USE_BUTTON
     buttonEnabled,
    #else
     0,
    #endif
    FavoritesManager::FavoritesRunning,
    FavoritesManager::rndCycle,
    random_on);
  
  #if defined(USE_NTP) || defined(USE_MANUAL_TIME_SETTING) || defined(GET_TIME_FROM_PHONE)
  char timeBuf[9];
  getFormattedTime(timeBuf);
  sprintf_P(outputBuffer, PSTR("%s %s"), outputBuffer, timeBuf);
  #else
  time_t currentTicks = millis() / 1000UL;
  sprintf_P(outputBuffer, PSTR("%s %02u:%02u:%02u"), outputBuffer, hour(currentTicks), minute(currentTicks), second(currentTicks));
  #endif
  #if USE_MP3_PLAYER
  sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, (uint8_t)eff_sound_on);
  //sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, (uint8_t)eff_volume);
  #endif  // USE_MP3_PLAYER
  #if GENERAL_DEBUG
  LOG.print ("Output Bufer ");
  LOG.println (outputBuffer);
  #endif
}

void sendAlarms(char *outputBuffer)
{
      char k[2];
    bool alarm_change = false;
      String configAlarm = readFile(F("config_alarm.json"), 512); 
  #if GENERAL_DEBUG
    LOG.println ("\nТекущие установки будильника");
      LOG.println(configAlarm);
  #endif
  strcpy_P(outputBuffer, PSTR("ALMS"));

  for (byte i = 0; i < 7; i++)
  {
  itoa ((i+1), k, 10);
    k[1] = 0;
    String a = "a" + String (k) ;
    String h = "h" + String (k) ;
    String m = "m" + String (k) ;
  if (alarms[i].State != (jsonReadtoInt(configAlarm, a)) || alarms[i].Time != (jsonReadtoInt(configAlarm, h)) * 60U + (jsonReadtoInt(configAlarm, m)))
  {
    alarm_change = true;
    jsonWrite(configAlarm, a, alarms[i].State);
    jsonWrite(configAlarm, h, (alarms[i].Time / 60U));
    jsonWrite(configAlarm, m, (alarms[i].Time % 60U));
  }
    sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, (uint8_t)alarms[i].State); 
  }

  for (byte i = 0; i < 7; i++)
  {
    sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, alarms[i].Time);
  }
  
  if (dawnMode != (jsonReadtoInt(configAlarm, "t")-1))
  {
    alarm_change = true;
    jsonWrite(configAlarm, "t", (dawnMode + 1));
  }
  sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, dawnMode + 1);
  if (alarm_change)
  {
    //writeFile("config_alarm.json", configAlarm );
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 1);

  #if GENERAL_DEBUG
    LOG.println ("\nНовые установки будильника сохранены в файл");
      LOG.println(configAlarm);
  #endif
  }
  DAWN_TIMEOUT = jsonReadtoInt(configAlarm, "after");
  DAWN_BRIGHT = constrain(jsonReadtoInt(configAlarm, "a_br"), 1, EFFECT_BRIGHTNESS_MAX);
}

void sendSunsets(char *outputBuffer)
{
      char k[2];
    bool sunset_change = false;
      String configSunset = readFile(F("config_sunset.json"), 512); 
  #if GENERAL_DEBUG
    LOG.println ("\nТекущие установки заката");
      LOG.println(configSunset);
  #endif
  strcpy_P(outputBuffer, PSTR("SUNS"));

  for (byte i = 0; i < 7; i++)
  {
  itoa ((i+1), k, 10);
    k[1] = 0;
    String a = "a" + String (k) ;
    String h = "h" + String (k) ;
    String m = "m" + String (k) ;
  if (sunsets[i].State != (jsonReadtoInt(configSunset, a)) || sunsets[i].Time != (jsonReadtoInt(configSunset, h)) * 60U + (jsonReadtoInt(configSunset, m)))
  {
    sunset_change = true;
    jsonWrite(configSunset, a, sunsets[i].State);
    jsonWrite(configSunset, h, (sunsets[i].Time / 60U));
    jsonWrite(configSunset, m, (sunsets[i].Time % 60U));
  }
    sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, (uint8_t)sunsets[i].State); 
  }

  for (byte i = 0; i < 7; i++)
  {
    sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, sunsets[i].Time);
  }
  
  if (sunsetMode != (jsonReadtoInt(configSunset, "t")-1))
  {
    sunset_change = true;
    jsonWrite(configSunset, "t", (sunsetMode + 1));
  }
  sprintf_P(outputBuffer, PSTR("%s %u"), outputBuffer, sunsetMode + 1);
  if (sunset_change)
  {
    //writeFile("config_sunset.json", configSunset );
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 1);

  #if GENERAL_DEBUG
    LOG.println ("\nНовые установки заката сохранены в файл");
      LOG.println(configSunset);
  #endif
  }
  SUNSET_BRIGHT = constrain(jsonReadtoInt(configSunset, "s_br"), 1, EFFECT_BRIGHTNESS_MAX);
}

void sendTimer(char *outputBuffer)
{
  sprintf_P(outputBuffer, PSTR("TMR %u %u %u"),
    TimerManager::TimerRunning,
    TimerManager::TimerOption,
   (TimerManager::TimerRunning ? (uint16_t)floor((TimerManager::TimeToFire - millis()) / 1000) : 0));
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

#if USE_MP3_PLAYER
void sendVolume(char *outputBuffer)
{
  sprintf_P(outputBuffer, PSTR("VOL %u %u"),
    eff_sound_on,
    eff_volume);
}
#endif  // USE_MP3_PLAYER
