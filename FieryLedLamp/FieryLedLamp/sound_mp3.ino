#if USE_MP3_PLAYER

#ifdef DF_PLAYER_IS_ORIGINAL
  #define ADVERT_TIMER_1 800UL        // Задержка между командой старт и адверт (если озвучка не играет)
  #define ADVERT_TIMER_2 700UL        // Задержка между произнесением минут и командой стоп (когда озвучка не играет)
  #define MP3_DELAY 500               // Задержка при настройке карты или флешки
#else
  #define ADVERT_TIMER_1 1000UL       // Задержка между командой старт и адверт (когда озвучка не играет)
  #define ADVERT_TIMER_2 2000UL       // Задержка между произнесением минут и командой стоп (когда озвучка не играет)
  #define MP3_DELAY 2000              // Задержка при настройке карты или флешки
#endif

#define MP3_READ_TIMEOUT  (500UL)

void mp3_setup()   {
  int16_t tmp;
  if ( first_entry == 5 ){
      first_entry = 0;
      delay(mp3_delay);
      send_command(0x0C,FEEDBACK,0,0);  //Сброс модуля
      mp3_timer = millis();
      #if GENERAL_DEBUG
      LOG.println(F("\n mp3 Reset "));
      #endif
      mp3_player_connect = 2;
      return;
  }
  if (mp3_receive_buf[3] == 0x3F) tmp = mp3_receive_buf[6];
  else tmp = -1;
  delay(mp3_delay);
// проверяем именно связь с модулем по реальному ответу на команду
  int16_t ack = send_command(0x06, FEEDBACK, 0, 0);   // поставить громкость 0, с ответом
  delay(mp3_delay);
  #ifndef CHECK_MP3_CONNECTION
  if (ack == -1) ack = 0;   // если проверка связи отключена, считаем что модуль есть
  #endif
// если модуль вообще не ответил — связи нет
if (ack == -1) {
  LOG.println(F("\nМР3 плеер не подключен или не отвечает\n"));
  mp3_player_connect = 0;
  return;
}
// если модуль ответил — связь есть
#ifdef DF_PLAYER_IS_ORIGINAL
  if (tmp == 1 || tmp == 3) {
    send_command(0x09, FEEDBACK, 0, 1);   // Flash
    delay(MP3_DELAY);
  }
  else if (tmp == 2) {
    send_command(0x09, FEEDBACK, 0, 2);   // SD
    delay(MP3_DELAY);
  }
#endif

  send_command(0x07, FEEDBACK, 0, Equalizer);
  delay(mp3_delay);
  send_command(0x06, FEEDBACK, 0, eff_volume);
  delay(mp3_delay);
// отдельно различаем: есть носитель или нет
if (tmp == 1 || tmp == 2 || tmp == 3) {
  mp3_player_connect = 4;
  LOG.print(F("\nMP3 плеер подключен. "));
  if (tmp == 2) LOG.println(F("Установлена SD-карта\n"));
  if (tmp == 1 || tmp == 3) LOG.println(F("Установлена флешка\n"));
  }
  else {
  mp3_player_connect = 5;
  LOG.println(F("\nMP3 плеер подключен, но SD-карта/флешка не установлена\n"));
 }
}

void play_time_ADVERT()   {
  if (mp3_player_connect == 4 && first_entry) {  
    if (first_entry !=2) {
       if (first_entry==1 && advert_hour) {
           advert_flag = true;
           first_entry = 3;
           {
           if (day_night) send_command(0x06,FEEDBACK,0,day_advert_volume);  //Громкость днём
           else send_command(0x06,FEEDBACK,0,night_advert_volume);  //Громкость ночью
           delay(mp3_delay);
           send_command(0x1A,FEEDBACK,0,1);     //mute on
           delay(mp3_delay);
           }
           if ((pause_on || mp3_stop) && !alarm_sound_flag) {  //+++++-----------+++++++++---------+++++++++
              send_command(0x06,FEEDBACK,0,0);
              delay(mp3_delay);
              send_command(0x0D,FEEDBACK,0,0);  //Старт
              delay(ADVERT_TIMER_1);
              if (day_night) send_command(0x06,FEEDBACK,0,day_advert_volume);  //Громкость днём
              else send_command(0x06,FEEDBACK,0,night_advert_volume);  //Громкость ночью
              delay(mp3_delay);
              send_command(0x1A,FEEDBACK,0,1);     //mute on
              delay(mp3_delay);
           }
           int pt_h=(uint8_t)((thisTime - thisTime % 60U) / 60U);
           if (pt_h==0) pt_h=24;
           send_command(0x13,FEEDBACK,0,pt_h);  //Старт Адверт №... Часы
           delay(mp3_delay);
           send_command(0x1A,FEEDBACK,0,0);         // Mute off
           delay(mp3_delay);
           //Serial.println ("Start ADVERT Hour");
           mp3_timer = millis();           
        }
        if (advert_hour && (millis() - mp3_timer > ADVERT_TIMER_H)) {
            delay(mp3_delay);
           advert_hour = false;
           int pt_m=(uint8_t)(thisTime % 60U);
            send_command(0x13,FEEDBACK,0,pt_m + 100);  //Старт Адверт №... минуты
           //Serial.println ("Start ADVERT Minute");
           mp3_timer = millis();
        }
        if (!advert_hour && millis() - mp3_timer > ADVERT_TIMER_M) {
            send_command(0x06,FEEDBACK,0,0);  //Громкость
            mp3_timer = millis();
            first_entry =2;
        }
    }
    else {
        if (millis() - mp3_timer > ADVERT_TIMER_2){
           advert_flag = false;
           first_entry =0;
           delay(mp3_delay);    
            if ((pause_on || mp3_stop) && !alarm_sound_flag) {
               send_command(0x0E,FEEDBACK,0,0);  //Пауза
               delay(mp3_delay);
            }
        if (dawnflag_sound) send_command(0x06,FEEDBACK,0,alarm_volume);  //Громкость будильника
        else send_command(0x06,FEEDBACK,0,eff_volume);  //Громкость эффектов
        delay(mp3_delay);
        }
     }
  }
}

void play_sound()   {
    if (!mp3_folder) {
        delay(mp3_delay);
        send_command(0x0E,FEEDBACK,0,0);  //Пауза
        mp3_stop = true;
        CurrentFolder = mp3_folder;
        CurrentFolder_last = CurrentFolder;
    }
    else {
        delay(mp3_delay);
         if ( mp3_folder >= 20 && mp3_folder <= 90 )
        {
            CurrentFolder = (uint8_t) random (mp3_folder, constrain (mp3_folder + 10, 20, 99));
        }
        else
        {
            CurrentFolder = mp3_folder;
        }
        if (((CurrentFolder_last != CurrentFolder) && set_mp3_play_now) || alarm_sound_flag) {
          send_command(0x17,FEEDBACK,0,CurrentFolder); // Включить непрерывное воспроизведение указанной папки
          delay(mp3_delay);
          //Serial.println ("play_sound");
          mp3_stop = false;
          CurrentFolder_last = CurrentFolder;
        }
        else if (((CurrentFolder_last != CurrentFolder) && set_mp3_play_now) || sunset_sound_flag){
          send_command(0x17,FEEDBACK,0,CurrentFolder); // Включить непрерывное воспроизведение указанной папки
          delay(mp3_delay);
          //Serial.println ("play_sound");
          mp3_stop = false;
          CurrentFolder_last = CurrentFolder;
        }
    }
    jsonWrite(configSetup, "fold_sel", CurrentFolder);
}

void mp3_loop()   {
  if (dawnFlag == 1) {                          // если наступает рассвет
      if (dawnflag_sound ) {
          if (alarm_sound_flag && (millis() - alarm_timer > 1000)) {
              alarm_timer = millis();
              send_command (0x06,FEEDBACK,0, min(((uint8_t)(dawnPosition/8)), alarm_volume)); //(0x06,FEEDBACK,0,constrain((uint8_t)(dawnPosition/8), 0, alarm_volume)); //Нарастание громкости в зависимости от стадии рассвета от 0 до alarm_volume
          }
          return;
     }
      send_command(0x0E,FEEDBACK,0,0);  //Пауза
      mp3_stop = true;
      dawnflag_sound = 1;
     if (alarm_sound_on) {
        delay(mp3_delay);
        mp3_folder = AlarmFolder;  // Папка будильника
        alarm_timer = millis();
        send_command(0x06,FEEDBACK,0,0);  //Громкость
        alarm_sound_flag = true;
        mp3_folder_last = mp3_folder;
        play_sound();
     }
    return;
  }
  else {
      if (dawnflag_sound) {
        send_command(0x06,FEEDBACK,0,eff_volume);  //Громкость
        delay(mp3_delay);
        alarm_sound_flag = false;
        dawnflag_sound = 0;
        send_command(0x0E,FEEDBACK,0,0);  //Пауза
        mp3_stop = true;
        delay(mp3_delay);
      }
  }

  if (sunsetFlag == 1) {                          // если наступает закат
      if (sunsetflag_sound ) {
          if (sunset_sound_flag && (millis() - sunset_timer > 1000)) {
              sunset_timer = millis();
              send_command (0x06,FEEDBACK,0, min(((uint8_t)(sunsetPosition/8)), sunset_volume)); //Уменьшение громкости в зависимости от стадии заката от sunset_volume до 0
          }
          return;
     }
      send_command(0x0E,FEEDBACK,0,0);  //Пауза
      mp3_stop = true;
      sunsetflag_sound = 1;
     if (sunset_sound_on) {
        delay(mp3_delay);
        mp3_folder = SunsetFolder;
        sunset_timer = millis();
        send_command(0x06,FEEDBACK,0,0);  //Громкость
        sunset_sound_flag = true;
        mp3_folder_last = mp3_folder;
        play_sound();
     }
    return;
  }
  else {
      if (sunsetflag_sound) {
        send_command(0x06,FEEDBACK,0,eff_volume);  //Громкость
        delay(mp3_delay);
        sunset_sound_flag = false;
        sunsetflag_sound = 0;
        send_command(0x0E,FEEDBACK,0,0);  //Пауза
        mp3_stop = true;
        delay(mp3_delay);
      }
  }
  if (ONflag && eff_sound_on) {
    set_mp3_play_now=true;  // Указывает, что надо играть сейчас мелодии  
    }
  else
    {
    set_mp3_play_now=false;  // Указывает, что не надо играть сейчас мелодии      
    }

  if (!mp3_stop && !set_mp3_play_now && !pause_on) {
    send_command(0x0E,FEEDBACK,0,0);  //Пауза
    pause_on = true;
    delay(mp3_delay);
  }   
  if (!mp3_stop && set_mp3_play_now && pause_on) {
    if (CurrentFolder == CurrentFolder_last)
        send_command(0x0D,FEEDBACK,0,0);  //Старт
    pause_on = false;
    delay(mp3_delay);
  }
 
  if ((CurrentFolder != CurrentFolder_last) && set_mp3_play_now) {
      send_command(0x17,FEEDBACK,0,CurrentFolder); // Включить непрерывное воспроизведение указанной папки
      mp3_stop = false;
      CurrentFolder_last = CurrentFolder;
  }

  if (mp3_folder_last != mp3_folder) {  // Проверка необходимости изменения папки озвучивания  (set_mp3_play_now && (mp3_folder_last != mp3_folder))
    mp3_folder_last = mp3_folder;
    play_sound();
  }
}

int16_t send_command(int8_t cmd, uint8_t feedback, uint8_t dat1, uint8_t dat2){
   uint8_t mp3_send_buf[8] = {0x7E, 0xFF, 06, 0x06, 00, 00, 00, 0xEF};
  mp3_send_buf[3] = cmd;  // Команда
  mp3_send_buf[4] = feedback; // 0x00 = Без ответа, 0x01 = с ответом (подтверждением)
  mp3_send_buf[5] = dat1; // параметр 1
  mp3_send_buf[6] = dat2; // параметр 2
  for (uint8_t i = 0; i < 8; i++)
  {
    mp3.write(mp3_send_buf[i]);
    delay(3);
  }
#ifdef MP3_DEBUG
  LOG.println();
  LOG.print(F("mp3_sending:"));
  for (uint8_t i=0; i<8; i++) {
    LOG.print(mp3_send_buf[i],HEX);
    LOG.print(F(" "));
  }
  LOG.println();
#endif  //MP3_DEBUG
  
  if (!feedback && (cmd < 0x30)) {
      return 0xFF00;
  }
  else if ( feedback && (cmd < 0x30)) {
        return read_command (MP3_READ_TIMEOUT);
  }
  else if (feedback && (cmd >= 0x30)) {
        if (read_command (MP3_READ_TIMEOUT) == -1) return -1;
        if (read_command (MP3_READ_TIMEOUT) == -1) return -1;
        return (((int16_t)mp3_receive_buf[5]) << 8) + mp3_receive_buf[6];
      }
  else if (!feedback && (cmd >= 0x30)) {
        if (read_command (MP3_READ_TIMEOUT) == -1) return -1;
        return (((int16_t)mp3_receive_buf[5]) << 8) + mp3_receive_buf[6];
      }
    return 0xEF00;
} 

int16_t read_command (uint32_t mp3_read_timeout) {
    int tmp;
    uint32_t tmr = millis();
    while (true) {
        tmp = mp3.read();
        if (tmp == 0x7E) break;
        if (millis() - tmr > mp3_read_timeout) return -1;
        delay(1);
    }

    mp3_receive_buf[0] = (uint8_t)tmp;

    for (uint8_t i = 1; i < 10; i++) {
        uint32_t btmr = millis();
        while (!mp3.available()) {
            if (millis() - btmr > mp3_read_timeout) return -1;
            delay(1);
        }
        tmp = mp3.read();
        if (tmp < 0) return -1;
        mp3_receive_buf[i] = (uint8_t)tmp;
    }

    if (mp3_receive_buf[2] == 6 && mp3_receive_buf[9] == 0xEF && mp3_receive_buf[3] != 0x40) {
        return (((int16_t)mp3_receive_buf[5]) << 8) + mp3_receive_buf[6];
    }
    return -1;
}
#endif
