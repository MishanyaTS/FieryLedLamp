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
#define MP3_CHECK_INTERVAL (60000UL)  // Период проверки связи во время работы
#define MP3_CHECK_TIMEOUT  (150UL)    // Сколько ждём ответ проверки без блокировки loop()
#define MP3_CHECK_MAX_FAILS (3U)      // Сколько подряд нет ответов, прежде чем считать плеер пропавшим


#define WEATHER_ADV_MIN_TEMP   (-45)
#define WEATHER_ADV_MAX_TEMP   45
#define WEATHER_ADV_FIRST_FILE 200
#define WEATHER_DESC_FIRST_FILE 300
#define WEATHER_TEMP_TIMER     ADVERT_TIMER_W
#define WEATHER_DESC_TIMER     ADVERT_TIMER_WDESC

uint16_t weatherTempTrack(int8_t temp) {
  temp = constrain(temp, WEATHER_ADV_MIN_TEMP, WEATHER_ADV_MAX_TEMP);
  return (uint16_t)(WEATHER_ADV_FIRST_FILE + temp - WEATHER_ADV_MIN_TEMP);
}

void weatherAdvertAddTrack(uint16_t track) {
  if (weather_advert_count >= (sizeof(weather_advert_tracks) / sizeof(weather_advert_tracks[0]))) return;
  for (uint8_t i = 0; i < weather_advert_count; i++) {
    if (weather_advert_tracks[i] == track) return;
  }
  weather_advert_tracks[weather_advert_count++] = track;
}

uint16_t weatherAdvertDescTrackByText(String s) {
  s.trim();
  s.toLowerCase();
  s.replace("ё", "е");

  if (s == "ясно") return 300;
  if (s == "малооблачно" || s == "переменная облачность") return 301;
  if (s == "облачно" || s == "облачно с прояснениями") return 302;
  if (s == "пасмурно") return 303;
  if (s == "дождь" || s == "идет дождь") return 304;
  if (s == "снег" || s == "идет снег") return 305;
  if (s == "туман") return 306;
  if (s == "гроза") return 307;
  if (s == "град") return 308;
  if (s == "смог" || s == "дымка") return 309;
  if (s == "пыльная мгла") return 310;
  if (s == "холодно") return 311;
  if (s == "прохладно") return 312;
  if (s == "комфортно") return 313;
  if (s == "тепло") return 314;
  if (s == "жарко") return 315;
  if (s == "очень жарко") return 316;
  if (s == "морозно") return 317;
  if (s == "мороз") return 318;
  if (s == "сильный мороз") return 319;
  if (s == "экстремальный мороз") return 320;
  if (s == "морось") return 321;
  if (s == "слабый дождь" || s == "небольшой дождь") return 322;
  if (s == "умеренный дождь") return 323;
  if (s == "сильный дождь") return 324;
  if (s == "затяжной сильный дождь" || s == "продолжительный сильный дождь") return 325;
  if (s == "ливень") return 326;
  if (s == "мокрый снег" || s == "дождь со снегом") return 327;
  if (s == "небольшой снег" || s == "слабый снег") return 328;
  if (s == "снегопад") return 329;
  if (s == "дождь, гроза" || s == "дождь с грозой") return 330;
  if (s == "гроза, град" || s == "гроза с градом") return 331;
  if (s == "малооблачно, слабый дождь" || s == "малооблачно, небольшой дождь") return 332;
  if (s == "малооблачно, дождь") return 333;
  if (s == "пасмурно, слабый дождь" || s == "значительная облачность, небольшой дождь") return 334;
  if (s == "пасмурно, сильный дождь" || s == "значительная облачность, сильный дождь") return 335;
  if (s == "пасмурно, сильный дождь, гроза" || s == "сильный дождь с грозой") return 336;
  if (s == "облачно, слабый дождь" || s == "облачно, небольшой дождь") return 337;
  if (s == "облачно, дождь") return 338;
  if (s == "пасмурно, мокрый снег" || s == "пасмурно, дождь со снегом") return 339;
  if (s == "малооблачно, небольшой снег" || s == "малооблачно, слабый снег") return 340;
  if (s == "малооблачно, снег") return 341;
  if (s == "пасмурно, небольшой снег" || s == "пасмурно, слабый снег") return 342;
  if (s == "пасмурно, снегопад") return 343;
  if (s == "облачно, небольшой снег" || s == "облачно, слабый снег") return 344;
  if (s == "облачно, снег") return 345;
  if (s == "пыльная буря") return 346;
  if (s == "песчаная мгла") return 347;
  if (s == "вулканический пепел") return 348;
  if (s == "шторм") return 349;
  if (s == "метель") return 350;
  if (s == "сильная метель") return 351;

  return 0;
}

void weatherAdvertAddDescTracks(const String& desc) {
  String s = desc;
  s.toLowerCase();
  s.replace("ё", "е");

  uint16_t exactTrack = weatherAdvertDescTrackByText(s);
  if (exactTrack) {
    weatherAdvertAddTrack(exactTrack);
    return;
  }

  if (s.indexOf("экстремаль") >= 0) weatherAdvertAddTrack(320);
  else if (s.indexOf("сильн") >= 0 && s.indexOf("мороз") >= 0) weatherAdvertAddTrack(319);
  else if (s.indexOf("морозно") >= 0) weatherAdvertAddTrack(317);
  else if (s.indexOf("мороз") >= 0) weatherAdvertAddTrack(318);
  else if (s.indexOf("очень") >= 0 && s.indexOf("жарк") >= 0) weatherAdvertAddTrack(316);
  else if (s.indexOf("жарк") >= 0) weatherAdvertAddTrack(315);
  else if (s.indexOf("тепл") >= 0) weatherAdvertAddTrack(314);
  else if (s.indexOf("комфорт") >= 0) weatherAdvertAddTrack(313);
  else if (s.indexOf("прохлад") >= 0) weatherAdvertAddTrack(312);
  else if (s.indexOf("холод") >= 0) weatherAdvertAddTrack(311);
  else if (s.indexOf("ясн") >= 0) weatherAdvertAddTrack(300);
  else if (s.indexOf("малооблач") >= 0 || s.indexOf("переменн") >= 0 || s.indexOf("прояснен") >= 0) weatherAdvertAddTrack(301);
  else if (s.indexOf("пасмур") >= 0 || s.indexOf("значительная облачность") >= 0) weatherAdvertAddTrack(303);
  else if (s.indexOf("облач") >= 0) weatherAdvertAddTrack(302);
  if (s.indexOf("гроз") >= 0) weatherAdvertAddTrack(307);
  if (s.indexOf("град") >= 0) weatherAdvertAddTrack(308);
  if (s.indexOf("затяж") >= 0 && s.indexOf("дожд") >= 0) weatherAdvertAddTrack(325);
  else if (s.indexOf("сильн") >= 0 && s.indexOf("дожд") >= 0) weatherAdvertAddTrack(324);
  else if (s.indexOf("умерен") >= 0 && s.indexOf("дожд") >= 0) weatherAdvertAddTrack(323);
  else if ((s.indexOf("слаб") >= 0 || s.indexOf("небольш") >= 0) && s.indexOf("дожд") >= 0) weatherAdvertAddTrack(322);
  else if (s.indexOf("лив") >= 0) weatherAdvertAddTrack(326);
  else if (s.indexOf("морос") >= 0) weatherAdvertAddTrack(321);
  else if (s.indexOf("дожд") >= 0) weatherAdvertAddTrack(304);
  if ((s.indexOf("мокр") >= 0 || s.indexOf("дождь со снег") >= 0) && s.indexOf("снег") >= 0) weatherAdvertAddTrack(327);
  else if ((s.indexOf("слаб") >= 0 || s.indexOf("небольш") >= 0) && s.indexOf("снег") >= 0) weatherAdvertAddTrack(328);
  else if (s.indexOf("снегопад") >= 0) weatherAdvertAddTrack(329);
  else if (s.indexOf("снег") >= 0 || s.indexOf("снеж") >= 0) weatherAdvertAddTrack(305);
  if (s.indexOf("метел") >= 0) weatherAdvertAddTrack((s.indexOf("сильн") >= 0) ? 351 : 350);
  if (s.indexOf("туман") >= 0) weatherAdvertAddTrack(306);
  if (s.indexOf("смог") >= 0 || s.indexOf("дымк") >= 0 || s.indexOf("мгла") >= 0) weatherAdvertAddTrack(309);
  if (s.indexOf("пыльн") >= 0 || s.indexOf("пыл") >= 0) weatherAdvertAddTrack(s.indexOf("бур") >= 0 ? 346 : 310);
  if (s.indexOf("пес") >= 0) weatherAdvertAddTrack(347);
  if (s.indexOf("пеп") >= 0 || s.indexOf("вулкан") >= 0) weatherAdvertAddTrack(348);
  if (s.indexOf("шторм") >= 0) weatherAdvertAddTrack(349);
}

void playWeatherAdvertTrack(uint16_t track) {
  send_command(0x13, FEEDBACK, (uint8_t)(track >> 8), (uint8_t)(track & 0xFF));
}

uint16_t weatherAdvertWaitTime() {
  if (weather_advert_index <= 1) return WEATHER_TEMP_TIMER;
  return WEATHER_DESC_TIMER;
}

void start_weather_temp_ADVERT(int8_t temp, bool speakDescription) {
  if (mp3_player_connect != 4 || advert_flag || weather_advert_flag) return;

  temp = constrain(temp, WEATHER_ADV_MIN_TEMP, WEATHER_ADV_MAX_TEMP);
  weather_advert_count = 0;
  weatherAdvertAddTrack(weatherTempTrack(temp));

  if (show_weather_desc && speakDescription) weatherAdvertAddDescTracks(currentCondition);

  weather_advert_index = 0;
  weather_advert_state = 1;
  weather_advert_flag = true;
  weather_advert_timer = 0;
}

void play_weather_temp_ADVERT() {
  if (mp3_player_connect != 4) {
    weather_advert_flag = false;
    weather_advert_state = 0;
    weather_advert_index = 0;
    weather_advert_count = 0;
    return;
  }
  if (!weather_advert_flag) return;

  if (weather_advert_state == 1) {
    if (weather_advert_index == 0) {
      if (day_night) send_command(0x06, FEEDBACK, 0, day_advert_volume);   // Громкость днём
      else           send_command(0x06, FEEDBACK, 0, night_advert_volume); // Громкость ночью
      delay(mp3_delay);
      send_command(0x1A, FEEDBACK, 0, 1); // mute on
      delay(mp3_delay);

      if ((pause_on || mp3_stop) && !alarm_sound_flag) {
        send_command(0x06, FEEDBACK, 0, 0);
        delay(mp3_delay);
        send_command(0x0D, FEEDBACK, 0, 0); // Старт
        delay(ADVERT_TIMER_1);
        if (day_night) send_command(0x06, FEEDBACK, 0, day_advert_volume);
        else           send_command(0x06, FEEDBACK, 0, night_advert_volume);
        delay(mp3_delay);
        send_command(0x1A, FEEDBACK, 0, 1); // mute on
        delay(mp3_delay);
      }

      playWeatherAdvertTrack(weather_advert_tracks[weather_advert_index++]);
      delay(mp3_delay);
      send_command(0x1A, FEEDBACK, 0, 0); // mute off
      delay(mp3_delay);
      weather_advert_timer = millis();
      return;
    }

    if (millis() - weather_advert_timer > weatherAdvertWaitTime()) {
      if (weather_advert_index < weather_advert_count) {
        playWeatherAdvertTrack(weather_advert_tracks[weather_advert_index++]);
        weather_advert_timer = millis();
      } else {
        send_command(0x06, FEEDBACK, 0, 0); // Громкость 0 перед завершением ADVERT
        weather_advert_timer = millis();
        weather_advert_state = 2;
      }
    }
    return;
  }

  if (weather_advert_state == 2 && millis() - weather_advert_timer > ADVERT_TIMER_2) {
    delay(mp3_delay);
    if ((pause_on || mp3_stop) && !alarm_sound_flag) {
      send_command(0x0E, FEEDBACK, 0, 0); // Пауза
      delay(mp3_delay);
    }
    if (dawnflag_sound)       send_command(0x06, FEEDBACK, 0, alarm_volume);  // Громкость будильника
    else if (sunsetflag_sound) send_command(0x06, FEEDBACK, 0, sunset_volume); // Громкость заката
    else                      send_command(0x06, FEEDBACK, 0, eff_volume);    // Громкость эффектов
    delay(mp3_delay);

    weather_advert_flag = false;
    weather_advert_state = 0;
    weather_advert_index = 0;
    weather_advert_count = 0;
  }
}

void mp3_clear_runtime_flags() {
  first_entry = 0;
  advert_flag = false;
  advert_hour = false;
  weather_advert_flag = false;
  weather_advert_state = 0;
  weather_advert_index = 0;
  weather_advert_count = 0;
  alarm_sound_flag = false;
  sunset_sound_flag = false;
  dawnflag_sound = 0;
  sunsetflag_sound = 0;
  set_mp3_play_now = false;
  mp3_stop = true;
  pause_on = true;
}

void mp3_send_command_nowait(uint8_t cmd, uint8_t feedback, uint8_t dat1, uint8_t dat2) {
  uint8_t mp3_send_buf[8] = {0x7E, 0xFF, 0x06, cmd, feedback, dat1, dat2, 0xEF};
  mp3.write(mp3_send_buf, sizeof(mp3_send_buf));   // UART отправит байты сам, без delay()
}

void mp3_periodic_check() {
#ifdef CHECK_MP3_CONNECTION
  static bool     checkWaiting = false;
  static uint8_t  checkPos = 0;
  static uint8_t  checkFails = 0;
  static uint32_t checkStarted = 0;

  uint32_t now = millis();

  if (advert_flag || weather_advert_flag || alarm_sound_flag || sunset_sound_flag || first_entry) {
    checkWaiting = false;
    checkPos = 0;
    return;
  }

  if (!checkWaiting) {
    if (now - mp3_check_timer < MP3_CHECK_INTERVAL) return;
    mp3_check_timer = now;

    // Убираем старые ответы от предыдущих команд, чтобы проверка читала только свой ответ.
    while (mp3.available()) mp3.read();

    mp3_send_command_nowait(0x06, 1, 0, eff_volume);
    checkStarted = now;
    checkPos = 0;
    checkWaiting = true;
    return;
  }

  while (mp3.available()) {
    int b = mp3.read();
    if (b < 0) break;
    if (checkPos == 0 && b != 0x7E) continue;
    mp3_receive_buf[checkPos++] = (uint8_t)b;

    if (checkPos >= sizeof(mp3_receive_buf)) {
      bool ok = (mp3_receive_buf[0] == 0x7E &&
                 mp3_receive_buf[2] == 0x06 &&
                 mp3_receive_buf[9] == 0xEF &&
                 mp3_receive_buf[3] != 0x40);

      checkWaiting = false;
      checkPos = 0;

      if (ok) {
        checkFails = 0;
      } else if (++checkFails >= MP3_CHECK_MAX_FAILS) {
        LOG.println(F("\nMP3 плеер пропал или не отвечает\n"));
        mp3_clear_runtime_flags();
        mp3_player_connect = 0;
        mp3_timer = millis();
        checkFails = 0;
      }
      return;
    }
  }

  if (now - checkStarted > MP3_CHECK_TIMEOUT) {
    checkWaiting = false;
    checkPos = 0;
    if (++checkFails >= MP3_CHECK_MAX_FAILS) {
      LOG.println(F("\nMP3 плеер пропал или не отвечает\n"));
      mp3_clear_runtime_flags();
      mp3_player_connect = 0;
      mp3_timer = millis();
      checkFails = 0;
    }
  }
#endif
}

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
  int16_t ack = send_command(0x06, 1, 0, 0);   // поставить громкость 0, с ответом. ВАЖНО: feedback=1, независимо от MP3_DEBUG
  delay(mp3_delay);
  #ifndef CHECK_MP3_CONNECTION
  if (ack == -1) ack = 0;   // если проверка связи отключена, считаем что модуль есть
  #endif
// если модуль вообще не ответил — связи нет
if (ack == -1) {
  LOG.println(F("\nМР3 плеер не подключен или не отвечает\n"));
  mp3_clear_runtime_flags();
  mp3_player_connect = 0;
  mp3_timer = millis();
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
  if (mp3_player_connect != 4) {
    // Если плеер отключили или он пропал во время озвучки, не зависаем в while(advert_flag).
    advert_flag = false;
    advert_hour = false;
    first_entry = 0;
    return;
  }

  if (first_entry) {  
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
        else if (sunsetflag_sound) send_command(0x06,FEEDBACK,0,sunset_volume);  //Громкость заката
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
