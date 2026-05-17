void User_setings ()  {
    
 BackupRestoreInit();
 
 HTTP.on("/favorit", handle_favorit);    // включить \ выключить переход кнопкой только по эффектам из выбранных в режиме Цикл и
 HTTP.on("/random_on", handle_random);  // случайных настроек эффектов в режиме цикл без сохранения в EEPROM
 HTTP.on("/print_time", handle_print_time); //Периодичность вывода времени бегущей строкой
 HTTP.on("/print_weather", handle_print_weather); // Периодичность вывода погоды бегущей строкой
#if USE_BUTTON
 HTTP.on("/save_btn_clicks", handle_save_btn_clicks); // Настройки действий по количеству нажатий на кнопку.
 HTTP.on("/button_on", handle_button_on);  // Вкл/Выкл
 HTTP.on("/button_type", handle_button_type); // Сенсорная / механическая кнопка
#endif
#if USE_TFT
 HTTP.on("/tft_clock_color", handle_tft_clock_color);   // Цвет часов на TFT
 HTTP.on("/tft_weather_color", handle_tft_weather_color); // Цвет погоды на TFT
 HTTP.on("/tft_ticker_on", handle_tft_ticker_on); // Включить бегущаю строку на TFT
 HTTP.on("/tft_ticker_color", handle_tft_ticker_color); // Цвет бегущаей строки на TFT
 HTTP.on("/tft_ticker_speed", handle_tft_ticker_speed); // Скорость бегущаей строки на TFT
 HTTP.on("/tft_ticker_period", handle_tft_ticker_period); // Период бегущаей строки на TFT
 HTTP.on("/tft_ticker_text", handle_tft_ticker_text); // Период бегущаей строки на TFT
#endif
 HTTP.on("/ESP_mode", handle_ESP_mode); // Установка ESP Mode
 HTTP.on("/eff_reset", handle_eff_reset);  //сброс настроек эффектов по умолчанию
 HTTP.on("/run_text", handle_run_text);  // Текст для бегущей строки
 HTTP.on("/night_time", handle_night_time);  // Параметры вывода времени бегущей строкой на выключенной лампе (яркость и время день,ночь) 
 HTTP.on("/effect_always", handle_effect_always);  // Не возобновлять работу эффектов
 HTTP.on("/timer5h", handle_timer5h);  // Автовыключение через 5 часов
 HTTP.on("/ntp", handle_ntp);  // Адрес NTP сервера
 HTTP.on("/eff_sel", handle_eff_sel);  // Выбор эффекта из списка
 HTTP.on("/eff", handle_eff);  // Следующий / Предыдущий
 HTTP.on("/br", handle_br);  // Яркость
 HTTP.on("/sp", handle_sp);  // Скорость
 HTTP.on("/sc", handle_sc);  // Масштаб / Цвет
 HTTP.on("/brm", handle_brm);  // Пошаговая яркость  минус
 HTTP.on("/brp", handle_brp);  // Пошаговая яркость плюс
 HTTP.on("/spm", handle_spm);  // Пошаговая  скорость минус
 HTTP.on("/spp", handle_spp);  // Пошаговая скорость  плюс
 HTTP.on("/scm", handle_scm);  // Пошаговый масштаб  минус
 HTTP.on("/scp", handle_scp);  // Пошаговый мвсштаб  плюс
 //HTTP.on("/tm", handle_tm);  // Смена темы страници (0 - светлая / 1 - тёмная)
 //HTTP.on("/PassOn", handle_PassOn); // Использовать (1) или нет (0) пароль для доступа к странице Начальных настроек
 HTTP.on("/Power", handle_Power);          // устройство вкл/выкл
 HTTP.on("/summer_time", handle_summer_time);  //Переход на летнее время 1 - да , 0 - нет
 HTTP.on("/time_always", handle_time_always);     // Выводить или нет время бегущей строкой(если задано) на не активной лампе
 HTTP.on("/weather_always", handle_weather_always);     // Выводить или нет погоду бегущей строкой(если задано) на не активной лампе
 HTTP.on("/show_weather_desc", handle_show_weather_desc); // Показывать описание погоды после температуры в бегущей строке
 HTTP.on("/timeZone", handle_time_zone);    // Установка смещения времени относительно GMT.
 HTTP.on("/alarm", handle_alarm);   // Установка будильника "рассвет"
 HTTP.on("/sunset", handle_sunset);   // Установка заката
 HTTP.on("/cycle_on", handle_cycle_on);   // Вкл/выкл режима Цикл
 HTTP.on("/time_eff", handle_time_eff);   // Время переключения цикла + Dispersion добавочное случайное время от 0 до disp
 HTTP.on("/rnd_cycle", handle_rnd_cycle);   // Перемешать выбранные или по порядку
 HTTP.on("/cycle_allwase", handle_cycle_allwase);   // Запускать режим цикл после выкл/вкл лампы или нет
 HTTP.on("/cycle_set", handle_cycle_set);   // Выбор эффектов для цикла
 HTTP.on("/eff_all", handle_eff_all);   // Выбрать все
 HTTP.on("/eff_clr", handle_eff_clr);   // сбросить Выбор
 HTTP.on("/timer", handle_timer);   // Запуск таймера выключения
 HTTP.on("/def", handle_def);   //  Установка настроек эффекта по умолчанию
 HTTP.on("/rnd", handle_rnd);   // Установка случайных настроек эффектов
 HTTP.on("/all_br", handle_all_br);  // Общая яркость
 #if USE_MULTIPLE_LAMPS_CONTROL
 HTTP.on("/multi", handle_multiple_lamp);  // Настройка управления несколькими лампами
 #endif //USE_MULTIPLE_LAMPS_CONTROL
 HTTP.on("/eff_save", handle_eff_save);  // Сохранить настройки эффектов в файл
 HTTP.on("/eff_read", handle_eff_read);  // Загрузить настройки эффектов из файла
 //HTTP.on("/alt", handle_alt_panel);   // Альтернативная главная web страница управления эффектами 
 HTTP.on("/get_time", get_time_manual);  // Синхронизация времени лампы с браузером на устройстве (телефоне)
 //HTTP.on("/index", handle_index);  // Начальная страница
 #if USE_MP3_PLAYER
 HTTP.on("/on_sound", handle_on_sound);  // Включить/Выключить звук эффектов
 HTTP.on("/vol", handle_volume);  // Громкость озвучивания эффектов
 HTTP.on("/on_alm_snd", handle_alarm_on_sound);  // Включить/Выключить звук будильника
 HTTP.on("/on_sun_snd", handle_sunset_on_sound);  // Включить/Выключить звук заката
 HTTP.on("/alm_vol", handle_alarm_volume);  // Громкость озвучивания будильника
 HTTP.on("/sun_vol", handle_sunset_volume);  // Громкость озвучивания заката
 HTTP.on("/alm_fold_sel", handle_alarm_fold_sel);  // Выбор папки для будильника
 HTTP.on("/sun_fold_sel", handle_sunset_fold_sel);  // Выбор папки для заката
 HTTP.on("/on_day_adv", handle_day_advert_on_sound);  // Включить/Выключить озвучивание времени днём
 HTTP.on("/on_night_adv", handle_night_advert_on_sound);  // Включить/Выключить озвучивание времени ночью
 HTTP.on("/on_alm_adv", handle_alarm_advert_sound_on);  // Включить/Выключить озвучивание времени будильником
 HTTP.on("/on_day_wadv", handle_day_weather_advert_on_sound);  // Включить/Выключить озвучивание температуры погоды днём
 HTTP.on("/on_night_wadv", handle_night_weather_advert_on_sound);  // Включить/Выключить озвучивание температуры погоды ночью
 HTTP.on("/on_alm_wadv", handle_alarm_weather_advert_sound_on);  // Включить/Выключить озвучивание температуры погоды будильником
 HTTP.on("/on_day_wdesc", handle_day_weather_desc_advert_on_sound);  // Включить/Выключить озвучивание описания погоды днём
 HTTP.on("/on_night_wdesc", handle_night_weather_desc_advert_on_sound);  // Включить/Выключить озвучивание описания погоды ночью
 HTTP.on("/on_alm_wdesc", handle_alarm_weather_desc_advert_sound_on);  // Включить/Выключить озвучивание описания погоды будильником
 HTTP.on("/day_vol", handle_day_advert_volume);  // Громкость озвучивания времени/погоды днём
 HTTP.on("/night_vol", handle_night_advert_volume);  // Громкость озвучивания времени/погоды ночью
 HTTP.on("/sound_set", handle_sound_set);     // Выбор привязанных папок для озвучивания эффектов
 HTTP.on("/track_down", handle_folder_down);  // Предыдущая папка
 HTTP.on("/track_up", handle_folder_up);      // Следующая папка
 HTTP.on("/fold_sel", handle_folder_select);  // Выбор папки озвучивания на главной странице
 HTTP.on("/eq", handle_equalizer);  // Эквалайзер
 HTTP.on("/test", handle_test); // Настройка таймингов DF-Playera (озвучивание времени)
 HTTP.on("/testw", handle_testw); // Настройка таймингов DF-Playera (озвучивание погоды)
 HTTP.on("/mp3_on", handle_mp3_on); // Включение/выключение MP3-плеера в настройках оборудования
 #endif
 HTTP.on("/tm1637_on", handle_tm1637_on); // Включение/выключение дисплея TM1637 в настройках оборудования
 HTTP.on("/tft_on", handle_tft_on); // Включение/выключение дисплея TFT в настройках оборудования
 HTTP.on("/ir_on", handle_ir_on); // Включение/выключение ИК-приёмника в настройках оборудования
 HTTP.on("/rtc_on", handle_rtc_on); // Включение/выключение модуля RTC в настройках оборудования
 HTTP.on("/cur_lim", handle_current_limit);  // выбор лимита тока матрицы
 HTTP.on("/m_t", handle_matrix_tipe);        // выбор типа матрицы
 HTTP.on("/m_o", handle_matrix_orientation); // Выбор ориентации марицы
 HTTP.on("/color_order", handle_color_order);
 HTTP.on("/matrix_size", handle_matrix_size); // Размер матрицы
 HTTP.on("/data_lines", handle_data_lines);   // Выбор 1/2 DATA-линий для матриц больше 1024 LED
 HTTP.on("/ssdp", handle_ssdp);  // Имя лампы
 HTTP.on("/res_to_def", handle_reset_to_default);  // Сброс всех настроек к "заводским"
 HTTP.on("/toe", handle_runing_text_over_effects );  // Выводить бегущую строку поверх эффектов
 HTTP.on("/spt", handle_spt);  // Скорость бегущей строки
 HTTP.on("/sct", handle_sct);  // Цвет бегущей строки
 HTTP.on("/ctf", handle_color_text_fon);  // Выводить бегущую строку на цветном фоне
 HTTP.on("/s_IP", handle_use_static_ip);  // Использовать для подключения к роутеру статичный IP адрес
 HTTP.on("/set_ip", handle_set_static_ip);  // Установка статичного IP адреса, шлюза, маски подсети и DNS сервера
 HTTP.on("/auto_bri", handle_auto_bri);  // Автоматическое понижение яркости в ночное время
 HTTP.on("/show_weather", handle_show_weather);         // Погода на TM1637
 #if USE_BLYNK
 HTTP.on("/blynk_token", handle_blynk_token); // Токен Blynk
 HTTP.on("/use_blynk", handle_use_blynk);   // Вкл/выкл Blynk 
 #endif
 HTTP.on("/button_status", HTTP_GET, handle_button_status);
 HTTP.on("/ir_status", HTTP_GET, handle_ir_status);
 HTTP.on("/tm1637_status", HTTP_GET, handle_tm1637_status);
 HTTP.on("/tft_status", HTTP_GET, handle_tft_status);
 HTTP.on("/rtc_status", HTTP_GET, handle_rtc_status);
 HTTP.on("/mp3_status", HTTP_GET, handle_mp3_status);
 HTTP.on("/multilamp_status", HTTP_GET, handle_multilamp_status);
 HTTP.on("/mqtt_status", HTTP_GET, handle_mqtt_status);
 HTTP.on("/blynk_status", HTTP_GET, handle_blynk_status);
 HTTP.on("/ota_status", HTTP_GET, handle_ota_status);
  
 #if USE_MQTT
 HTTP.on("/mqtt_set", handle_mqtt_set);  // Параметры настроек MQTT
 HTTP.on("/mqtt_on", handle_mqtt_on);  // Использовать MQTT клиент
 HTTP.on("/mqtt_prd", handle_mqtt_period); // Период публикации ответа лампы (0 – 60 секунд)
 #endif
 HTTP.on("/ssidap", HTTP_GET, []() {   // Получаем SSID AP со страницы
     jsonWrite(configSetup, "ssidAP", HTTP.arg("ssidAP"));
     jsonWrite(configSetup, "passwordAP", HTTP.arg("passwordAP"));
     saveConfig();                             // Функция сохранения строки конфигурации в файл
     HTTP.send(200, F("text/plain"), F("OK")); // отправляем ответ о выполнении
 });
 // --------------------Получаем SSID со страницы
  HTTP.on("/ssid", HTTP_GET, []() {
  jsonWrite(configSetup, "ssid", HTTP.arg("ssid"));
  jsonWrite(configSetup, "password", HTTP.arg("password"));
  jsonWrite(configSetup, "ssid2",         HTTP.arg("ssid2"));
  jsonWrite(configSetup, "password2",     HTTP.arg("password2"));
  jsonWrite(configSetup, "ssid3",         HTTP.arg("ssid3"));
  jsonWrite(configSetup, "password3",     HTTP.arg("password3"));
  jsonWrite(configSetup, "TimeOut", HTTP.arg("TimeOut").toInt()); 
  ESP_CONN_TIMEOUT = jsonReadtoInt(configSetup, "TimeOut");
  saveConfig();                 // Функция сохранения строки конфигурации в файл
  HTTP.send(200, F("text/plain"), F( "OK")); // отправляем ответ о выполнении
  });
  
#if USE_IR_RECEIVER
  HTTP.on("/ir_set", HTTP_GET, []() {
  if (!HTTP.hasArg("key")) {
    HTTP.send(400, F("text/plain"), F("Missing key"));
    return;
  }
  String key = HTTP.arg("key");
  key.trim();
  if (key.length() == 0) {
    HTTP.send(400, F("text/plain"), F("Bad key"));
    return;
  }
  String configIR = readFile(F("config_ir.json"), 4096);
  if (configIR == "Failed" || configIR == "Large") {
    HTTP.send(500, F("text/plain"), F("IR config error"));
    return;
  }
  bool changed = false;
  if (HTTP.hasArg("code")) {
    uint32_t code = (uint32_t)HTTP.arg("code").toInt();
    jsonWrite(configIR, key.c_str(), (int)code);
    changed = true;
  }
  if (HTTP.hasArg("comment")) {
    String c = HTTP.arg("comment");
    c.trim();
    if (c.length() > 32) c.remove(32);
    String cKey = key + F("_comment");
    jsonWrite(configIR, cKey.c_str(), c);
    changed = true;
  }
  if (changed) {
    writeFile(F("config_ir.json"), configIR);
    IR_LoadConfigFromFile();   // apply now
  }
  HTTP.send(200, F("text/plain"), F("OK"));
});

HTTP.on("/ir_learn", HTTP_GET, []() {
    if (!HTTP.hasArg("key")) {
      HTTP.send(400, F("text/plain"), F("Missing key"));
      return;
    }
    String key = HTTP.arg("key");
    key.trim();
    if (!IR_LearnStart(key.c_str())) {
      HTTP.send(400, F("text/plain"), F("Bad key"));
      return;
    }
    HTTP.send(200, F("text/plain"), F("OK"));
  });

  HTTP.on("/ir_learn_status", HTTP_GET, []() {
    String out;
    IR_LearnGetStatusJson(out);
    HTTP.send(200, F("application/json"), out);
  });
  
  HTTP.on("/ir_learn_page", HTTP_GET, []() {
    if (!HTTP.hasArg("key")) {
      HTTP.send(400, F("text/plain"), F("Missing key"));
      return;
    }
    String key = HTTP.arg("key");
    key.trim();
    if (!IR_LearnStart(key.c_str())) {
      HTTP.send(400, F("text/plain"), F("Bad key"));
      return;
    }
    String html;
    html.reserve(1800);
    html += F("<!doctype html><html><head><meta charset='utf-8'>");
    html += F("<meta name='viewport' content='width=device-width,initial-scale=1'>");
    html += F("<title>IR Learn</title></head><body style='font-family:sans-serif;background:#111;color:#eee;padding:16px'>");
    html += F("<h3>Обучение кнопки</h3>");
    html += F("<p>Нажмите кнопку на пульте в течение 15 секунд...</p>");
    html += F("<p><b>Ключ:</b> ");
    html += key;
    html += F("</p><pre id='st' style='background:#222;padding:10px;border-radius:8px'>ожидаю...</pre>");
    html += F("<script>");
    html += F("const st=document.getElementById('st');");
    html += F("const t0=Date.now();");
    html += F("async function poll(){");
    html += F(" try{");
    html += F("  const r=await fetch('/ir_learn_status');");
    html += F("  const j=await r.json();");
    html += F("  if(j.last && j.last!=0){ st.textContent='Записано: '+j.last+'\\nВозврат...'; setTimeout(()=>location.href='/?setup_IR',700); return; }");
    html += F("  if(!j.active){ st.textContent='Таймаут/отмена. Возврат...'; setTimeout(()=>location.href='/?setup_IR',700); return; }");
    html += F("  st.textContent='Ожидаю нажатие...';");
    html += F(" }catch(e){ st.textContent='Ошибка: '+e; }");
    html += F(" if(Date.now()-t0<16000) setTimeout(poll,500); else {location.href='/?setup_IR';}");
    html += F("}");
    html += F("poll();");
    html += F("</script></body></html>");
    HTTP.send(200, F("text/html"), html);
  });
#endif //USE_IR_RECEIVER

    HTTP.on("/?setup_IR", HTTP_GET, []() {
      #if !USE_IR_RECEIVER
    HTTP.send(404, "text/plain", "Настройки пульта отключены в прошивке");
      #else
    handleFileRead("/setup_IR.htm");
      #endif
  });

    HTTP.on("/?setup_sound", HTTP_GET, []() {
      #ifndef USE_MP3_PLAYER
    HTTP.send(404, "text/plain", "Настройки звука отключены в прошивке");
      #else
    handleFileRead("/setup_sound.htm");
      #endif
  });

   HTTP.on("/?setup_multilamp", HTTP_GET, []() {
     #ifndef USE_MULTIPLE_LAMPS_CONTROL
   HTTP.send(404, "text/plain", "Управление несколькими лампами отключено в прошивке");
     #else
   handleFileRead("/setup_multilamp.htm");
     #endif
  });

  HTTP.on("/?setup_mqtt", HTTP_GET, []() {
     #ifndef USE_MQTT
   HTTP.send(404, "text/plain", "MQTT отключено в прошивке");
     #else
   handleFileRead("/setup_mqtt.htm");
     #endif
  });

  HTTP.on("/update", HTTP_GET, []() {                                            // Запустить страницу обновления по WEB (<IP>/update)
    #ifndef USE_OTA
  HTTP.send(404, "text/plain", "Обновление по воздуху (OTA) отключено в прошивке");
    #else
  handleFileRead("/update.htm");
    #endif
  });

  #if (USE_TM1637 || USE_TFT)
  HTTP.on("/save_display_times", HTTP_GET, []() {
  int clockVal   = HTTP.arg("clock").toInt();
  int weatherVal = HTTP.arg("weather").toInt();
  if (clockVal < 3 || clockVal > 300 ||
      weatherVal < 3 || weatherVal > 300) {
    HTTP.send(400, "text/plain", "Invalid value (3-300)");
    return;
  }
  String configDisplay = readFile(F("config_display.json"), 1024);
  if (configDisplay == F("Failed") || configDisplay == F("Large")) configDisplay = F("{}");
  jsonWrite(configDisplay, "clock_time", clockVal);
  jsonWrite(configDisplay, "weather_time", weatherVal);
  writeFile(F("config_display.json"), configDisplay);
  CLOCK_SHOW_INTERVAL   = (uint32_t)clockVal * 1000UL;
  WEATHER_SHOW_INTERVAL = (uint32_t)weatherVal * 1000UL;
  displaySwitchTimer = millis();   // сброс таймера
  HTTP.send(200, "text/plain", "OK");
});
#endif

    HTTP.on("/get_settings", HTTP_GET, []() {
    DynamicJsonDocument doc(4096);
    deserializeJson(doc, configSetup);
    String output;
    serializeJson(doc, output);
    HTTP.send(200, "application/json", output);
  });

  HTTP.on("/get_weather", HTTP_GET, []() {
  DynamicJsonDocument doc(1024);
  String out;
#if (USE_WEATHER == 0)
  doc["text"] = "—";
  doc["temp"] = -999;
  doc["init"] = false;
  doc["city"] = "";
  doc["provider"] = "";
  serializeJson(doc, out);
  HTTP.send(200, "application/json", out);
  return;
#endif

  if (!inClockWeatherMode) {
    doc["text"] = "—";
    doc["temp"] = -999;
    doc["init"] = false;
    doc["city"] = weatherCity;
    doc["provider"] = actualYandex ? "yandex" : "openweather";
    serializeJson(doc, out);
    HTTP.send(200, "application/json", out);
    return;
  }

  if (currentTemp > -999) {
    String source = preferYandex ? "Яндекс" : "OpenWeather";
    String text = source + ": " + String((int)round(currentTemp)) + "°C";
    if (currentCondition.length() > 0) {
      text += ", " + currentCondition;
    }
    doc["text"] = text;
    doc["temp"] = (int)round(currentTemp);
  } else {
    doc["text"] = "—";
    doc["temp"] = -998;
  }

  doc["init"] = inClockWeatherMode;
  doc["city"] = weatherCity;
  doc["provider"] = actualYandex ? "yandex" : "openweather";

  serializeJson(doc, out);
  HTTP.send(200, "application/json", out);
});

  HTTP.on("/save_weather_param", HTTP_GET, []() {
    if (!HTTP.hasArg("key") || !HTTP.hasArg("value")) {
      HTTP.send(400, "text/plain", "Missing key or value");
      return;
    }
    String key = HTTP.arg("key");
    String value = HTTP.arg("value");
    LOG.printf("[SAVE] %s = %s\n", key.c_str(), value.c_str());

    if (key == "show_weather") {
      String configHardware = readFile(F("config_hardware.json"), 2048);
      inClockWeatherMode = (value == "1");
      jsonWrite(configHardware, "show_weather", inClockWeatherMode ? 1 : 0);
      writeFile(F("config_hardware.json"), configHardware);
      if (inClockWeatherMode && WiFi.status() == WL_CONNECTED) {
        weatherUpdateTimer = millis() - WEATHER_UPDATE_INTERVAL + 1000;
      }
      HTTP.send(200, "text/plain", "OK");
      return;
    }

    jsonWrite(configSetup, key, value);
    saveConfig();

    if (key == "openweather_key")          weatherApiKey = value;
   else if (key == "yandex_geo") yandexGeoId = value;
    else if (key == "city")                weatherCity = value;
    else if (key == "prefer_yandex") preferYandex = (value == "1");
    HTTP.send(200, "text/plain", "OK");
  });

// IP адрес в модальном окне "Статусы устройств"
HTTP.on("/wifi_ip", HTTP_GET, []() {
  DynamicJsonDocument doc(256);
  String ip;

  // Проверяем, включён ли режим точки доступа (AP)
  if (WiFi.getMode() & WIFI_AP || WiFi.softAPgetStationNum() > 0) {
    ip = WiFi.softAPIP().toString();
    if (ip == "0.0.0.0") ip = "192.168.4.1";
  } else {
    // Режим клиента (STA)
    ip = WiFi.localIP().toString();
    if (ip == "0.0.0.0") {
      ip = "Не получен IP";
    }
  }

  doc["ip"] = ip;

  String response;
  serializeJson(doc, response);
  HTTP.send(200, "application/json; charset=utf-8", response);
});

HTTP.on("/features", HTTP_GET, []() {
    DynamicJsonDocument doc(512);
    doc["button"] = !!USE_BUTTON;
    doc["tm1637"] = !!USE_TM1637;
    doc["mp3"] = !!USE_MP3_PLAYER;
    doc["weather"] = !!USE_WEATHER;
    doc["multilamp"] = !!USE_MULTIPLE_LAMPS_CONTROL;
    doc["mqtt"] = !!USE_MQTT;
    doc["ota"] = !!USE_OTA;
    String response;
    serializeJson(doc, response);
    HTTP.send(200, "application/json", response);
  });

HTTP.on("/heap", HTTP_GET, []() {
  DynamicJsonDocument doc(1280);

  uint32_t freeHeap    = ESP.getFreeHeap();
  uint32_t totalHeap   = 0;
  uint32_t freePsram   = 0;
  uint32_t totalPsram  = 0;
  bool     hasPsram    = false;
  String   chip        = "unknown";

  totalHeap = ESP.getHeapSize();
  if (psramFound()) {
    totalPsram = ESP.getPsramSize();
    freePsram  = ESP.getFreePsram();
    hasPsram   = true;
  }
  chip = "ESP32-S3";

  doc["chip"]        = chip;
  doc["free"]        = freeHeap;
  doc["total_dram"]  = totalHeap;
  doc["free_psram"]  = freePsram;
  doc["total_psram"] = totalPsram;
  doc["total"]       = totalHeap + totalPsram;
  doc["free_total"]  = freeHeap + freePsram;
  doc["psram"]       = hasPsram;
  doc["uptime"]      = millis() / 1000UL;

  String response;
  serializeJson(doc, response);
  HTTP.send(200, "application/json", response);
});

// Инфа о прошивке
  HTTP.on("/version", HTTP_GET, []() {
    DynamicJsonDocument doc(1024);
    
    // Версия прошивки (определяется в файле Constants.h)
    #ifdef VERSION
      doc["version"] = VERSION;
    #else
      doc["version"] = "";
    #endif

    // Доп инфа
    doc["chip"] = "ESP32-S3";

    doc["cpu_freq"] = ESP.getCpuFreqMHz();
    doc["flash_size"] = ESP.getFlashChipSize() / 1024 / 1024;
    doc["free_heap"] = ESP.getFreeHeap();
    doc["uptime"] = millis() / 1000UL; 
    doc["wifi_mode"] = (WiFi.getMode() == WIFI_AP) ? "AP" : 
                       (WiFi.getMode() == WIFI_STA) ? "Station" : "AP+Station";
    doc["ip_address"] = WiFi.localIP().toString();
    doc["lamp_on"] = ONflag;
    doc["current_effect"] = currentMode;
    doc["effect_count"] = MODE_AMOUNT;

    // Статус модулей
    #if USE_TM1637
      doc["tm1637_enabled"] = true;
    #else
      doc["tm1637_enabled"] = false;
    #endif
    #if USE_RTC
      doc["rtc_enabled"] = true;
    #else
      doc["rtc_enabled"] = false;
    #endif
    #if USE_MP3_PLAYER
      doc["mp3_enabled"] = true;
    #else
      doc["mp3_enabled"] = false;
    #endif   
    #if USE_MQTT
      doc["mqtt_enabled"] = true;
    #else
      doc["mqtt_enabled"] = false;
    #endif
    
    String response;
    serializeJson(doc, response);
    HTTP.send(200, "application/json; charset=utf-8", response);
});

    // Проверка версии
HTTP.on("/check_update", HTTP_GET, []() {
  DynamicJsonDocument doc(256);
  doc["current_version"] = String(F(VERSION));

  String resp;
  serializeJson(doc, resp);
  HTTP.send(200, "application/json; charset=utf-8", resp);
});

  OtaPackageInit();

}

void handle_ssdp()   {
    jsonWrite(configSetup, "SSDP", HTTP.arg("ssdp"));
    SSDP.setName(jsonRead(configSetup, "SSDP"));
    saveConfig();                              // Функция сохранения данных во Flash
    LAMP_NAME = jsonRead(configSetup, "SSDP");
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_favorit() {    
  jsonWrite(configSetup, "favorit", HTTP.arg("favorit").toInt());
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
  Favorit_only = jsonReadtoInt(configSetup, "favorit");
  HTTP.send(200, F("text/plain"), F("OK"));
 }

void handle_random() { 
  jsonWrite(configSetup, "random_on", HTTP.arg("random_on").toInt());
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
  random_on = jsonReadtoInt(configSetup, "random_on");
  HTTP.send(200, F("text/plain"), F("OK"));
 }
 
void handle_print_time() {    
  jsonWrite(configSetup, "print_time", HTTP.arg("print_time").toInt()); 
  PRINT_TIME = jsonReadtoInt(configSetup, "print_time");
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
  HTTP.send(200, F("text/plain"), F("OK"));
 }

 void handle_print_weather() {
  jsonWrite(configSetup, "print_weather", HTTP.arg("print_weather").toInt());
  PRINT_WEATHER = jsonReadtoInt(configSetup, "print_weather");
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
  HTTP.send(200, F("text/plain"), F("OK"));
}

#if USE_BUTTON
void handle_save_btn_clicks() {
    String configHardware = readFile(F("config_hardware.json"), 2048);
    btn_click_power = HTTP.arg("btn_click_power").toInt();
    btn_click_next = HTTP.arg("btn_click_next").toInt();
    btn_click_prev = HTTP.arg("btn_click_prev").toInt();
    btn_click_action4 = HTTP.arg("btn_click_action4").toInt();
    btn_click_ip = HTTP.arg("btn_click_ip").toInt();
    btn_click_time = HTTP.arg("btn_click_time").toInt();
    btn_click_esp_mode = HTTP.arg("btn_click_esp_mode").toInt();
    btn_click_sound = HTTP.arg("btn_click_sound").toInt();
    btn_click_weather = HTTP.arg("btn_click_weather").toInt();
    jsonWrite(configHardware, "btn_click_power", btn_click_power);
    jsonWrite(configHardware, "btn_click_next", btn_click_next);
    jsonWrite(configHardware, "btn_click_prev", btn_click_prev);
    jsonWrite(configHardware, "btn_click_action4", btn_click_action4);
    jsonWrite(configHardware, "btn_click_ip", btn_click_ip);
    jsonWrite(configHardware, "btn_click_time", btn_click_time);
    jsonWrite(configHardware, "btn_click_esp_mode", btn_click_esp_mode);
    jsonWrite(configHardware, "btn_click_sound", btn_click_sound);
    jsonWrite(configHardware, "btn_click_weather", btn_click_weather);
    writeFile(F("config_hardware.json"), configHardware);
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}
#endif

#if USE_TFT
void handle_tft_clock_color() {
  String configDisplay = readFile(F("config_display.json"), 1024);
  if (configDisplay == F("Failed") || configDisplay == F("Large")) configDisplay = F("{}");
  jsonWrite(configDisplay, "tft_clock_color", HTTP.arg("tft_clock_color").toInt());
  tft_clock_color = jsonReadtoInt(configDisplay, "tft_clock_color");
  writeFile(F("config_display.json"), configDisplay);
  HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_tft_weather_color() {
  String configDisplay = readFile(F("config_display.json"), 1024);
  if (configDisplay == F("Failed") || configDisplay == F("Large")) configDisplay = F("{}");
  jsonWrite(configDisplay, "tft_weather_color", HTTP.arg("tft_weather_color").toInt());
  tft_weather_color = jsonReadtoInt(configDisplay, "tft_weather_color");
  writeFile(F("config_display.json"), configDisplay);
  HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_tft_ticker_on() {
  String configDisplay = readFile(F("config_display.json"), 1024);
  if (configDisplay == F("Failed") || configDisplay == F("Large")) configDisplay = F("{}");
  jsonWrite(configDisplay, "tft_ticker_on", HTTP.arg("tft_ticker_on").toInt());
  tft_ticker_on = jsonReadtoInt(configDisplay, "tft_ticker_on");
  writeFile(F("config_display.json"), configDisplay);
  HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_tft_ticker_color() {
  String configDisplay = readFile(F("config_display.json"), 1024);
  if (configDisplay == F("Failed") || configDisplay == F("Large")) configDisplay = F("{}");
  jsonWrite(configDisplay, "tft_ticker_color", HTTP.arg("tft_ticker_color").toInt());
  tft_ticker_color = jsonReadtoInt(configDisplay, "tft_ticker_color");
  writeFile(F("config_display.json"), configDisplay);
  HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_tft_ticker_speed() {
  String configDisplay = readFile(F("config_display.json"), 1024);
  if (configDisplay == F("Failed") || configDisplay == F("Large")) configDisplay = F("{}");
  jsonWrite(configDisplay, "tft_ticker_speed", HTTP.arg("tft_ticker_speed").toInt());
  tft_ticker_speed = jsonReadtoInt(configDisplay, "tft_ticker_speed");
  writeFile(F("config_display.json"), configDisplay);
  HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_tft_ticker_period() {
  String configDisplay = readFile(F("config_display.json"), 1024);
  if (configDisplay == F("Failed") || configDisplay == F("Large")) configDisplay = F("{}");
  jsonWrite(configDisplay, "tft_ticker_period", HTTP.arg("tft_ticker_period").toInt());
  tft_ticker_period = jsonReadtoInt(configDisplay, "tft_ticker_period");
  writeFile(F("config_display.json"), configDisplay);
  HTTP.send(200, F("text/plain"), F("OK"));
}
  
void handle_tft_ticker_text() {
  String s = HTTP.arg("tft_ticker_text");
  if (s.length() > 120) s.remove(120);
  String configDisplay = readFile(F("config_display.json"), 1024);
  if (configDisplay == F("Failed") || configDisplay == F("Large")) configDisplay = F("{}");
  jsonWrite(configDisplay, "tft_ticker_text", s);
  s.toCharArray(TFTTickerText, s.length() + 1);
  writeFile(F("config_display.json"), configDisplay);
  HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}
#endif

void handle_button_on() {
    String configHardware = readFile(F("config_hardware.json"), 2048);
    buttonEnabled = HTTP.arg("button_on").toInt();
    jsonWrite(configHardware, "button_on", buttonEnabled);
    writeFile(F("config_hardware.json"), configHardware);
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_button_type() {
    String configHardware = readFile(F("config_hardware.json"), 2048);
    button_type = HTTP.arg("button_type").toInt();
    if (button_type > 1) button_type = 1;
    jsonWrite(configHardware, "button_type", button_type);
    writeFile(F("config_hardware.json"), configHardware);
    if (button_type) {
      touch.setType(LOW_PULL);
      touch.setDebounce(BUTTON_SET_DEBOUNCE_SENSORY);
    } else {
      touch.setType(HIGH_PULL);
      touch.setDebounce(BUTTON_SET_DEBOUNCE_MECHANICAL);
    }
    touch.setDirection(NORM_OPEN);
    touch.setTimeout(BUTTON_CLICK_TIMEOUT);
    touch.setClickTimeout(BUTTON_CLICK_TIMEOUT);
    touch.setStepTimeout(BUTTON_STEP_TIMEOUT);
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_ESP_mode() { 
  jsonWrite(configSetup, "ESP_mode", HTTP.arg("ESP_mode").toInt());
  saveConfig();  
  espMode = jsonReadtoInt(configSetup, "ESP_mode");
  HTTP.send(200, F("text/plain"), F("OK"));
 }

void handle_eff_reset() {    
    restoreSettings();
    updateSets();
    jsonWrite(configSetup, "br", modes[currentMode].Brightness);
    jsonWrite(configSetup, "sp", modes[currentMode].Speed);
    jsonWrite(configSetup, "sc", modes[currentMode].Scale);    
    showWarning(CRGB::Blue, 2000U, 500U);                    // мигание синим цветом 2 секунды
    #if USE_BLYNK
    updateRemoteBlynkParams();
    #endif
    HTTP.send(200, F("text/plain"), F("OK"));
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL   
 }

void handle_run_text ()  {
    jsonWrite(configSetup, "run_text", HTTP.arg("run_text"));
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    (jsonRead(configSetup, "run_text")).toCharArray (TextTicker, (jsonRead(configSetup, "run_text")).length()+1);
    #if GENERAL_DEBUG
    LOG.print("\nTextTicker = ");
    uint8_t i=0;
    while (TextTicker[i]!=0)
    {
        LOG.print (TextTicker[i],HEX);
        LOG.print (' ');
        i++;
    }
    LOG.println();
    #endif
    HTTP.send(200, F("text/plain"), F("OK")); // отправляем ответ о выполнении
 }

void handle_night_time ()  {
    jsonWrite(configSetup, "night_time", HTTP.arg("night_time").toInt());
    jsonWrite(configSetup, "night_bright", HTTP.arg("night_bright").toInt());
    jsonWrite(configSetup, "day_time", HTTP.arg("day_time").toInt());
    jsonWrite(configSetup, "day_bright", HTTP.arg("day_bright").toInt());
    saveConfig();
    NIGHT_HOURS_START = 60U * jsonReadtoInt(configSetup, "night_time");
    NIGHT_HOURS_BRIGHTNESS = jsonReadtoInt(configSetup, "night_bright");
    NIGHT_HOURS_STOP = 60U * jsonReadtoInt(configSetup, "day_time");
    DAY_HOURS_BRIGHTNESS = jsonReadtoInt(configSetup, "day_bright");
    getBrightnessForPrintTime();
    if(ONflag && !dawnFlag && !sunsetFlag)
        SetBrightness(modes[currentMode].Brightness);
    #if USE_TM1637
    clockTicker_blink();
    #endif
    #if USE_TFT
    TFT_ApplyBrightnessNow();
    #endif
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("text/plain"), F("OK"));
 }

void handle_effect_always ()  {
    jsonWrite(configSetup, "effect_always", HTTP.arg("effect_always").toInt());
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    DONT_TURN_ON_AFTER_SHUTDOWN = jsonReadtoInt(configSetup, "effect_always");
    HTTP.send(200, F("text/plain"), F("OK"));
 }

void handle_timer5h ()  {
    jsonWrite(configSetup, "timer5h", HTTP.arg("timer5h").toInt());
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    AUTOMATIC_OFF_TIME = (30UL * 60UL * 1000UL) * ( uint32_t )(jsonReadtoInt(configSetup, "timer5h"));
    if (ONflag ) {
      TimerManager::TimerRunning = true;
      if (!AUTOMATIC_OFF_TIME)
          TimerManager::TimeToFire = millis() + AUTOMATIC_OFF_TIME;
      else
          TimerManager::TimeToFire = millis() + (24UL*60UL*60UL*1000UL); //Если 0 то не выключать (установить 24 часа)
    }
    HTTP.send(200, F("text/plain"), F("OK"));
 }
 
void handle_ntp ()  {
    jsonWrite(configSetup, "ntp", HTTP.arg("ntp"));
    saveConfig();
    (jsonRead(configSetup, "ntp")).toCharArray (NTP_ADDRESS, (jsonRead(configSetup, "ntp")).length()+1);
    HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_eff_sel () {
    uint8_t temp = (HTTP.arg("eff_sel").toInt());
    jsonWrite(configSetup, "eff_sel", temp);
    currentMode = temp;
    jsonWrite(configSetup, "br", modes[currentMode].Brightness);
    jsonWrite(configSetup, "sp", modes[currentMode].Speed);
    jsonWrite(configSetup, "sc", modes[currentMode].Scale);
    SetBrightness(modes[currentMode].Brightness);
    loadingFlag = true;
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
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL   
}

void handle_eff () {
    uint8_t temp = jsonReadtoInt (configSetup, "eff_sel");
    if (HTTP.arg("eff").toInt())  {
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
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
}

void handle_br ()  {
    jsonWrite(configSetup, "br", HTTP.arg("br").toInt());
    modes[currentMode].Brightness = jsonReadtoInt(configSetup, "br");
    SetBrightness(modes[currentMode].Brightness);
    #if GENERAL_DEBUG
    LOG.printf_P(PSTR("Новое значение яркости: %d\n"), modes[currentMode].Brightness);
    #endif
     HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}")); 
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

void handle_sp ()  {
    jsonWrite(configSetup, "sp", HTTP.arg("sp").toInt());
    modes[currentMode].Speed = jsonReadtoInt(configSetup, "sp");
    loadingFlag = true;    // Перезапуск Эффекта
    #if GENERAL_DEBUG
    LOG.printf_P(PSTR("Новое значение скорости: %d\n"), modes[currentMode].Speed);
    #endif
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
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

void handle_sc ()  {
    jsonWrite(configSetup, "sc", HTTP.arg("sc").toInt());
    modes[currentMode].Scale = jsonReadtoInt(configSetup, "sc");
    loadingFlag = true;  // Перезапуск Эффекта
    #if GENERAL_DEBUG
    LOG.printf_P(PSTR("Новое значение Масштаба / Цвета: %d\n"), modes[currentMode].Scale);
    #endif
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
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

void handle_brm ()   {
    modes[currentMode].Brightness = constrain(modes[currentMode].Brightness - 1, 1, 255);
    jsonWrite(configSetup, "br", modes[currentMode].Brightness);
    SetBrightness(modes[currentMode].Brightness);
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
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

void handle_brp ()   {
    modes[currentMode].Brightness = constrain(modes[currentMode].Brightness + 1, 1, 255);
    jsonWrite(configSetup, "br", modes[currentMode].Brightness);
    SetBrightness(modes[currentMode].Brightness);
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
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

void handle_spm ()   {
    modes[currentMode].Speed = constrain(modes[currentMode].Speed - 1, 1, 255);
    jsonWrite(configSetup, "sp", modes[currentMode].Speed);
    loadingFlag = true;  // Перезапуск Эффекта
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
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

void handle_spp ()   {
    modes[currentMode].Speed = constrain(modes[currentMode].Speed + 1, 1, 255);
    jsonWrite(configSetup, "sp", modes[currentMode].Speed);
    loadingFlag = true;  // Перезапуск Эффекта
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
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

void handle_scm ()   {
    modes[currentMode].Scale = constrain(modes[currentMode].Scale - 1, 1, 100);
    jsonWrite(configSetup, "sc", modes[currentMode].Scale);
    loadingFlag = true;  // Перезапуск Эффекта
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
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

void handle_scp ()   {
    modes[currentMode].Scale = constrain(modes[currentMode].Scale + 1, 1, 100);
    jsonWrite(configSetup, "sc", modes[currentMode].Scale);
    loadingFlag = true;  // Перезапуск Эффекта
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
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
/*
void handle_tm ()   {
    bool flg = false;
    jsonWrite(configSetup, "tm", HTTP.arg("tm").toInt());
    if (jsonReadtoInt(configSetup, "tm")) flg = FileCopy (F("/css/dark/build.css.gz") , F("/css/build.css.gz"));
    else flg = FileCopy (F("/css/light/build.css.gz") , F("/css/build.css.gz"));
    if (flg) {
       HTTP.send(200, F("text/plain"), F("OK"));
       saveConfig();
    }
    else HTTP.send(404, F("text/plain"), "File not found");  
}

void handle_PassOn ()   {
    jsonWrite(configSetup, "PassOn", HTTP.arg("PassOn").toInt());
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
    saveConfig();
}
*/
void handle_Power ()  {
    uint8_t tmp;
    if (dawnFlag == 1) {
      manualOff = true;
      dawnFlag = 2;
      #if USE_TM1637
      clockTicker_blink();
      #endif
      SetBrightness(modes[currentMode].Brightness);
      changePower();
    }
    else if (sunsetFlag == 1){
      manualsOff = true;
      sunsetFlag = 2;
      #if USE_TM1637
      clockTicker_blink();
      #endif
      SetBrightness(modes[currentMode].Brightness);
      changePower();
    }
    else {    
    tmp = HTTP.arg("Power").toInt();
    if (tmp == 2) jsonReadtoInt(configSetup, "Power") == 0? tmp = 1 : tmp = 0;
      jsonWrite(configSetup, "Power", tmp);
      ONflag = tmp;
        if (!ONflag)  {
            // Немедленное выключение
            changePower(); // Выключаем матрицу сразу
            timeout_save_file_changes = millis() - SAVE_FILE_DELAY_TIMEOUT; // Сбрасываем таймер для немедленного сохранения
            save_file_changes = 7;
            Save_File_Changes();
        } else {
          // Включение лампы: загружаем настройки из EEPROM
            //EepromManager::EepromGet(modes);
            timeout_save_file_changes = millis();
            bitSet(save_file_changes, 0);
    changePower();
    loadingFlag = true;
        }
    }
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
    #if USE_MULTIPLE_LAMPS_CONTROL
    if (ONflag) {
        repeat_multiple_lamp_control=true;
    }
    else {
        multiple_lamp_control ();
    }
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
    #if USE_MQTT
    if (espMode == 1U)
    {
      MqttManager::needToPublish = true;
    }
    #endif
}

void handle_summer_time() {
    #ifdef USE_NTP
      jsonWrite(configSetup, "Summer_Time", HTTP.arg("Summer_Time").toInt()); 
      saveConfig();
      summerTime.offset = winterTime.offset + jsonReadtoInt(configSetup, "Summer_Time") * 60;
      localTimeZone.setRules (summerTime, winterTime);
    #endif
    jsonWrite(configSetup, "time", (Get_Time(getCurrentLocalTime())));
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
 }
 
void handle_time_always() {
    jsonWrite(configSetup, "time_always", HTTP.arg("time_always").toInt());
    time_always = jsonReadtoInt(configSetup, "time_always");
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("text/plain"), F("OK"));
 }

 void handle_weather_always() {
    jsonWrite(configSetup, "weather_always", HTTP.arg("weather_always").toInt());
    weather_always = jsonReadtoInt(configSetup, "weather_always");
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("text/plain"), F("OK"));
 }

#if USE_WEATHER
void handle_show_weather_desc() {
    show_weather_desc = HTTP.arg("show_weather_desc").toInt();
    jsonWrite(configSetup, "show_weather_desc", show_weather_desc);
    timeout_save_file_changes = millis();
    bitSet(save_file_changes, 0);
    HTTP.send(200, F("text/plain"), F("OK"));
}
#endif
 
void handle_time_zone() {     // Установка параметров времянной зоны 
    #ifdef USE_NTP
    jsonWrite(configSetup, "timezone", HTTP.arg("timeZone").toInt()); // Получаем значение timezone из запроса конвертируем в int сохраняем
    saveConfig();
    winterTime.offset = jsonReadtoInt(configSetup, "timezone") * 60;
    summerTime.offset = winterTime.offset + jsonReadtoInt(configSetup, "Summer_Time") * 60;
    localTimeZone.setRules (summerTime, winterTime);
    #endif
    jsonWrite(configSetup, "time", (Get_Time(getCurrentLocalTime())));
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_alarm ()  { 
    char i[3];
    String configAlarm = readFile(F("config_alarm.json"), 512); 
    bool saveRequest = HTTP.hasArg("t") || HTTP.hasArg("after") || HTTP.hasArg("a_br"); 
    for (uint8_t k=0; k<7; k++) {
        itoa(k + 1, i, 10);
        //i[1] = 0;
        String a = "a" + String (i) ;
        String h = "h" + String (i) ;
        String m = "m" + String (i) ;
        //сохранение параметров в строку
        if (saveRequest){  
        jsonWrite(configAlarm, a, HTTP.arg(a).toInt());
        jsonWrite(configAlarm, h, HTTP.arg(h).toInt());
        jsonWrite(configAlarm, m, HTTP.arg(m).toInt());
        }
    //сохранение установок будильника
    alarms[k].State = (jsonReadtoInt(configAlarm, a));
    alarms[k].Time = (jsonReadtoInt(configAlarm, h)) * 60 + (jsonReadtoInt(configAlarm, m));
        esp_task_wdt_reset();
        yield();
    }
    if (!first_entry) {
       jsonWrite(configAlarm, "t", HTTP.arg("t").toInt());
       jsonWrite(configAlarm, "after", HTTP.arg("after").toInt());
       jsonWrite(configAlarm, "a_br", HTTP.arg("a_br").toInt());
    } 
    dawnMode = jsonReadtoInt(configAlarm, "t")-1;
    DAWN_TIMEOUT = jsonReadtoInt(configAlarm, "after");
    DAWN_BRIGHT = jsonReadtoInt(configAlarm, "a_br");
    if (!first_entry) {
      manualOff = false;
      dawnFlag = 0;
      dawnCounter = 0;
      for (uint8_t j = 0U; j < 6U; j++) {
        dawnColor[j] = 0;
      }
    }

    if (saveRequest)
        {
         writeFile(F("config_alarm.json"), configAlarm );
        }
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_sunset ()  { 
    char i[3];
    String configSunset = readFile(F("config_sunset.json"), 512); 
    bool saveRequest = HTTP.hasArg("t") || HTTP.hasArg("s_br"); 
    for (uint8_t k=0; k<7; k++) {
        itoa(k + 1, i, 10);
        //i[1] = 0;
        String a = "a" + String (i) ;
        String h = "h" + String (i) ;
        String m = "m" + String (i) ;
        //сохранение параметров в строку
        if (saveRequest){  
        jsonWrite(configSunset, a, HTTP.arg(a).toInt());
        jsonWrite(configSunset, h, HTTP.arg(h).toInt());
        jsonWrite(configSunset, m, HTTP.arg(m).toInt());
        }
    //сохранение установок будильника
    sunsets[k].State = (jsonReadtoInt(configSunset, a));
    sunsets[k].Time = (jsonReadtoInt(configSunset, h)) * 60 + (jsonReadtoInt(configSunset, m));
        esp_task_wdt_reset();
        yield();
    }
    if (!first_entry) {
       jsonWrite(configSunset, "t", HTTP.arg("t").toInt());
       jsonWrite(configSunset, "s_br", HTTP.arg("s_br").toInt());
    } 
    sunsetMode = jsonReadtoInt(configSunset, "t")-1;
    SUNSET_BRIGHT = jsonReadtoInt(configSunset, "s_br");
    if (!first_entry) {
      manualsOff = false;
      sunsetFlag = 0;
      sunsetCounter = 0;
      for (uint8_t j = 0U; j < 6U; j++) {
        sunsetColor[j] = 0;
      }
    }

    if (saveRequest)
     {
   writeFile(F("config_sunset.json"), configSunset );
    }
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void save_alarms()   {
    char k[2];
    bool alarm_change = false;
    String configAlarm = readFile(F("config_alarm.json"), 512); 
    #if GENERAL_DEBUG
     LOG.println (F("\nТекущие установки будильника"));
     LOG.println(configAlarm);
    #endif
    esp_task_wdt_reset();
    for (byte i = 0; i < 7; i++) {
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
        yield();
    }
    if (dawnMode != (jsonReadtoInt(configAlarm, "t")-1)) {
        alarm_change = true;
        jsonWrite(configAlarm, "t", (dawnMode + 1));
    }
    jsonWrite(configAlarm, "after", DAWN_TIMEOUT);
    jsonWrite(configAlarm, "a_br", DAWN_BRIGHT);  
    if (alarm_change) {
        writeFile(F("config_alarm.json"), configAlarm );
        #if GENERAL_DEBUG
        LOG.println (F("\nНовые установки будильника сохранены в файл"));
        LOG.println(configAlarm);
        #endif
    }
}

void save_sunsets()   {
    char k[2];
    bool sunset_change = false;
    String configSunset = readFile(F("config_sunset.json"), 512); 
    #if GENERAL_DEBUG
     LOG.println (F("\nТекущие установки заката"));
     LOG.println(configSunset);
    #endif
    esp_task_wdt_reset();
    for (byte i = 0; i < 7; i++) {
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
        yield();
    }
    if (sunsetMode != (jsonReadtoInt(configSunset, "t")-1)) {
        sunset_change = true;
        jsonWrite(configSunset, "t", (sunsetMode + 1));
    }
    jsonWrite(configSunset, "s_br", SUNSET_BRIGHT);  
    if (sunset_change) {
        writeFile(F("config_sunset.json"), configSunset );
        #if GENERAL_DEBUG
        LOG.println (F("\nНовые установки заката сохранены в файл"));
        LOG.println(configSunset);
        #endif
    }
}

void handle_cycle_on()  {  // Вкл/выкл режима Цикл
    uint8_t tmp;
    tmp = HTTP.arg("cycle_on").toInt();
    if (tmp == 2) jsonReadtoInt(configSetup, "cycle_on") == 0? tmp = 1 : tmp = 0;
    if (ONflag && tmp)   {
        jsonWrite(configSetup, "cycle_on", 1);
        FavoritesManager::FavoritesRunning = 1;
        EepromManager::EepromPut(modes);
    }
    else   {
        FavoritesManager::FavoritesRunning = 0;
        FavoritesManager::nextModeAt = 0;
        jsonWrite(configSetup, "cycle_on", 0);
    }
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));  //HTTP.send(200, F("text/plain"), F("OK"));
    #if USE_MQTT
    if (espMode == 1U)
    {
      MqttManager::needToPublish = true;
    }
    #endif
}

void handle_time_eff ()  {  // Время переключения цикла + Dispersion добавочное случайное время от 0 до disp
    jsonWrite(configSetup, "time_eff", HTTP.arg("time_eff").toInt());
    FavoritesManager::Interval = jsonReadtoInt(configSetup, "time_eff");    
    jsonWrite(configSetup, "disp", HTTP.arg("disp").toInt());
    FavoritesManager::Dispersion = jsonReadtoInt(configSetup, "disp");    
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_rnd_cycle ()  {  // Перемешать выбранные или по порядку
    jsonWrite(configSetup, "rnd_cycle", HTTP.arg("rnd_cycle").toInt());
    FavoritesManager::rndCycle = jsonReadtoInt(configSetup, "rnd_cycle");
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_cycle_allwase ()  {  // Запускать режим цыкл после выкл/вкл лампы или нет
    jsonWrite(configSetup, "cycle_allwase", HTTP.arg("cycle_allwase").toInt());
    FavoritesManager::UseSavedFavoritesRunning = jsonReadtoInt(configSetup, "cycle_allwase");
    if (!ONflag && !FavoritesManager::UseSavedFavoritesRunning)   {
        FavoritesManager::FavoritesRunning = 0;
        jsonWrite(configSetup, "cycle_on", 0);
    }
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));  //HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_eff_all ()   {  //Выбрать все эффекты
    char i[4];
    String configCycle = readFile(F("config_cycle.json"), 2048); 
    // подготовка  строк с именами полей json 
    esp_task_wdt_reset();
    for (uint8_t k=0; k<MODE_AMOUNT; k++) {
        itoa ((k), i, 10);
        String e = "e" + String (i) ;
        //сохранение параметров в строку
        jsonWrite(configCycle, e, 1U);
        yield();
    }
    writeFile(F("config_cycle.json"), configCycle );
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_eff_clr ()   {  //очистить все эффекты
      char i[4];
      String configCycle = readFile(F("config_cycle.json"), 2048); 
      // подготовка  строк с именами полей json 
      esp_task_wdt_reset();
      for (uint8_t k=0; k<MODE_AMOUNT; k++)
      {
        itoa ((k), i, 10);
        String e = "e" + String (i) ;
        //сохранение параметров в строку
        jsonWrite(configCycle, e, 0U);
        yield();
      }
    writeFile(F("config_cycle.json"), configCycle );
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_cycle_set ()  {  // Выбор эффектов для Цикла 
      char i[4];
      String configCycle = readFile(F("config_cycle.json"), 2048); 
      /*#if GENERAL_DEBUG
      LOG.println (F("\nВыбор эффектов для Цикла"));
      LOG.println(configCycle);
      #endif*/
      // подготовка  строк с именами полей json file
      esp_task_wdt_reset();
      for (uint8_t k=0; k<MODE_AMOUNT; k++) {
       itoa ((k), i, 10);
          String e = "e" + String (i) ;
           //сохранение параметров в строку
        if (!first_entry)  
        jsonWrite(configCycle, e, HTTP.arg(e).toInt());
        //сохранение выбранных эффектов для Цикла
        FavoritesManager::FavoriteModes[k] = jsonReadtoInt(configCycle, e);
        yield();
        }
     /*#if GENERAL_DEBUG
      LOG.println (F("\nВыбор эффектов для Цикла после обработки"));
      LOG.println(configCycle);
     #endif*/     
      if (!first_entry)
        {
         writeFile(F("config_cycle.json"), configCycle );
        }
         HTTP.send(200, F("text/plain"), F("OK"));
}

void cycle_get ()  { // сохранение выбранных эффектов в файл
      char i[4];
      bool cycle_change = false;
      String configCycle = readFile(F("config_cycle.json"), 2048); 
      /*#if GENERAL_DEBUG
      LOG.println (F("\nВыбор эффектов для Цикла"));
      LOG.println(configCycle);
      #endif*/
      // подготовка  строк с именами полей json file
      esp_task_wdt_reset();
      for (uint8_t k=0; k<MODE_AMOUNT; k++) {
         itoa ((k), i, 10);
         String e = "e" + String (i) ;
           //передача параметров из массива в строку json если значение в памяти не равно значению в файле
          if (FavoritesManager::FavoriteModes[k] != jsonReadtoInt(configCycle, e)) 
          {
            jsonWrite(configCycle, e, FavoritesManager::FavoriteModes[k]);
            cycle_change = true;
          }
          yield();
        }
    if (cycle_change)    {
        writeFile(F("config_cycle.json"), configCycle );
        /*#if GENERAL_DEBUG
        LOG.println (F("\nНовый выбор эффектов для Цикла сохранен в файл"));
        LOG.println(configCycle);
        #endif*/
      }     
}

void handle_timer ()   {  // Установка таймера 
    uint8_t tmp;
    tmp = HTTP.arg("timer").toInt();
    if (tmp != jsonReadtoInt(configSetup, "timer"))   {
       jsonWrite(configSetup, "timer", tmp);
       saveConfig();
    }
    jsonWrite(configSetup, "tmr", 1);
    TimerManager::TimeToFire = millis() + tmp * 60UL * 1000UL;
    TimerManager::TimerRunning = true;    
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}")); //HTTP.send(200, F("application/json"), "{\"title\":\"Запущен\",\"class\":\"btn btn-block btn-warning\"}");
}

void handle_def ()   { // Сброс настроек текущего эффекта по умолчанию
    setModeSettings();
    updateSets();    
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
}

void handle_rnd ()   { // Установка случайных настроек текущему эффекту
    selectedSettings = 1U;
    updateSets();
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
}

void handle_all_br ()   {  //Общая яркость
    jsonWrite(configSetup, "all_br", HTTP.arg("all_br").toInt());
    uint8_t ALLbri = jsonReadtoInt(configSetup, "all_br");
    esp_task_wdt_reset();
    for (uint8_t i = 0; i < MODE_AMOUNT; i++) {
        modes[i].Brightness = ALLbri;    
      }
    jsonWrite(configSetup, "br", ALLbri);
    FastLED.setBrightness(ALLbri);
    loadingFlag = true;
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
}

#if USE_MULTIPLE_LAMPS_CONTROL

void handle_multiple_lamp () {
    String str;
    String configMultilamp = readFile(F("config_multilamp.json"), 512);
    jsonWrite(configMultilamp, "ml1", HTTP.arg("ml1").toInt());
    jsonWrite(configMultilamp, "ml2", HTTP.arg("ml2").toInt());
    jsonWrite(configMultilamp, "ml3", HTTP.arg("ml3").toInt());
    jsonWrite(configMultilamp, "ml4", HTTP.arg("ml4").toInt());
    jsonWrite(configMultilamp, "ml5", HTTP.arg("ml5").toInt());
    jsonWrite(configMultilamp, "host1", HTTP.arg("host1"));
    jsonWrite(configMultilamp, "host2", HTTP.arg("host2"));
    jsonWrite(configMultilamp, "host3", HTTP.arg("host3"));
    jsonWrite(configMultilamp, "host4", HTTP.arg("host4"));
    jsonWrite(configMultilamp, "host5", HTTP.arg("host5"));
    jsonWrite(configMultilamp, "comment1", HTTP.arg("comment1"));
    jsonWrite(configMultilamp, "comment2", HTTP.arg("comment2"));
    jsonWrite(configMultilamp, "comment3", HTTP.arg("comment3"));   
    jsonWrite(configMultilamp, "comment4", HTTP.arg("comment4"));
    jsonWrite(configMultilamp, "comment5", HTTP.arg("comment5"));  
    writeFile(F("config_multilamp.json"), configMultilamp );
    ml1 = jsonReadtoInt(configMultilamp, "ml1");
    ml2 = jsonReadtoInt(configMultilamp, "ml2");
    ml3 = jsonReadtoInt(configMultilamp, "ml3");
    ml4 = jsonReadtoInt(configMultilamp, "ml4");
    ml5 = jsonReadtoInt(configMultilamp, "ml5");
    str = jsonRead (configMultilamp, "host1");
    str.toCharArray (Host1, str.length() + 1);
    str = jsonRead (configMultilamp, "host2");
    str.toCharArray (Host2, str.length() + 1);
    str = jsonRead (configMultilamp, "host3");
    str.toCharArray (Host3, str.length() + 1);
    str = jsonRead (configMultilamp, "host4");
    str.toCharArray (Host4, str.length() + 1);
    str = jsonRead (configMultilamp, "host5");
    str.toCharArray (Host5, str.length() + 1);
    #if USE_MP3_PLAYER
    send_sound = HTTP.arg("s_s").toInt();
    jsonWrite(configSetup, "s_s", send_sound);
    send_eff_volume = HTTP.arg("s_e_v").toInt();
    if (!send_sound) {
      send_eff_volume = 0;
    }
    jsonWrite(configSetup, "s_e_v", send_eff_volume);
    #endif // USE_MP3_PLAYER
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void multilamp_get ()   {
    String str;
    String configMultilamp = readFile(F("config_multilamp.json"), 512);
    ml1 = jsonReadtoInt(configMultilamp, "ml1");
    ml2 = jsonReadtoInt(configMultilamp, "ml2");
    ml3 = jsonReadtoInt(configMultilamp, "ml3");
    ml4 = jsonReadtoInt(configMultilamp, "ml4");
    ml5 = jsonReadtoInt(configMultilamp, "ml5");
    str = jsonRead (configMultilamp, "host1");
    str.toCharArray (Host1, str.length() + 1);
    str = jsonRead (configMultilamp, "host2");
    str.toCharArray (Host2, str.length() + 1);
    str = jsonRead (configMultilamp, "host3");
    str.toCharArray (Host3, str.length() + 1);
    str = jsonRead (configMultilamp, "host4");
    str.toCharArray (Host4, str.length() + 1);
    str = jsonRead (configMultilamp, "host5");
    str.toCharArray (Host5, str.length() + 1);
}

void multiple_lamp_control ()   {
    
    char outputBuffer[38];
    
  if (connect)   {
    if ( ml1 )   {
      #if USE_MP3_PLAYER
      if (send_sound && !send_eff_volume) {
      sprintf_P(outputBuffer, PSTR("MULTI,%u,%u,%u,%u,%u,%u"),
        (uint8_t)ONflag,
        currentMode,
        modes[currentMode].Brightness,
        modes[currentMode].Speed,
        modes[currentMode].Scale,
        CurrentFolder);
      }
    else if (send_sound && send_eff_volume) {
      sprintf_P(outputBuffer, PSTR("MULTI,%u,%u,%u,%u,%u,%u,%u,%u"),
        (uint8_t)ONflag,
        currentMode,
        modes[currentMode].Brightness,
        modes[currentMode].Speed,
        modes[currentMode].Scale,
        eff_sound_on,
        eff_volume,
        CurrentFolder);
      }
      else {
        sprintf_P(outputBuffer, PSTR("MULTI,%u,%u,%u,%u,%u"),
        (uint8_t)ONflag,
        currentMode,
        modes[currentMode].Brightness,
        modes[currentMode].Speed,
        modes[currentMode].Scale);
      }
      #else
      sprintf_P(outputBuffer, PSTR("MULTI,%u,%u,%u,%u,%u"),
       (uint8_t) ONflag,
        currentMode,
        modes[currentMode].Brightness,
        modes[currentMode].Speed,
        modes[currentMode].Scale);
      #endif
      Udp.beginPacket(Host1,localPort);
      Udp.print(outputBuffer);
      Udp.endPacket();
      #if GENERAL_DEBUG
      LOG.print (F("Передача MULTI на IP "));
      LOG.print (Host1);
      LOG.print (F("  "));
      LOG.println (outputBuffer);
      #endif
    }
    
    if ( ml2 )   {
      Udp.beginPacket(Host2,localPort);
      Udp.print(outputBuffer);
      Udp.endPacket();
    #if GENERAL_DEBUG
      LOG.print (F("Передача MULTI на IP "));
      LOG.print (Host2);
      LOG.print (F("  "));
      LOG.println (outputBuffer);
    #endif
    }
    
    if ( ml3 )   {
      Udp.beginPacket(Host3,localPort);
      Udp.print(outputBuffer);
      Udp.endPacket();
    #if GENERAL_DEBUG
      LOG.print (F("Передача MULTI на IP "));
      LOG.print (Host3);
      LOG.print (F("  "));
      LOG.println (outputBuffer);
    #endif
    }

     if ( ml4 )   {
      Udp.beginPacket(Host4,localPort);
      Udp.print(outputBuffer);
      Udp.endPacket();
    #if GENERAL_DEBUG
      LOG.print (F("Передача MULTI на IP "));
      LOG.print (Host4);
      LOG.print (F("  "));
      LOG.println (outputBuffer);
    #endif
    }
  
    if ( ml5 )   {
      Udp.beginPacket(Host5,localPort);
      Udp.print(outputBuffer);
      Udp.endPacket();
    #if GENERAL_DEBUG
      LOG.print (F("Передача MULTI на IP "));
      LOG.print (Host5);
      LOG.print (F("  "));
      LOG.println (outputBuffer);
    #endif
    }
    outputBuffer[0] = '\0';
  } 
}
#endif //USE_MULTIPLE_LAMPS_CONTROL

void handle_eff_save ()   {
    LittleFS.begin();
    File file = LittleFS.open(F("/effect.ini"),"w");
    if (file)   {
        for (uint8_t i = 0; i < MODE_AMOUNT; i++) {
           file.write (modes[i].Brightness);
           file.write (modes[i].Speed);
           file.write (modes[i].Scale);
           yield();
        }
        #if GENERAL_DEBUG
        LOG.println (F("Настройки эффектов сохранены в файл"));
        #endif //GENERAL_DEBUG
        showWarning(CRGB::Blue, 2000U, 500U);                    // мигание синим цветом 2 секунды
        esp_task_wdt_reset();
        yield();
    }
    else   {
        #if GENERAL_DEBUG
        LOG.println (F("Не удалось сохранить настройки эффектов в файл"));
        #endif //GENERAL_DEBUG
    }
    file.close();
    HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_eff_read ()   {
    LittleFS.begin();
    File file = LittleFS.open(F("/effect.ini"),"r");
    if (file)   {
        uint16_t file_size = file.size();
        if ((file_size/3) < MODE_AMOUNT) file_size -= 6;
        esp_task_wdt_reset();
        for (uint8_t i = 0; i < (file_size/3); i++) {
           modes[i].Brightness = file.read ();
           modes[i].Speed = file.read ();
           modes[i].Scale = file.read ();
           yield();
        }
        #if GENERAL_DEBUG
        LOG.println (F("Настройки эффектов прочитаны из файла и применены"));
        #endif //GENERAL_DEBUG
        showWarning(CRGB::Blue, 2000U, 500U);                    // мигание синим цветом 2 секунды
        loadingFlag = true;  // Перезапуск Эффекта
        jsonWrite(configSetup, "br", modes[currentMode].Brightness);
        jsonWrite(configSetup, "sp", modes[currentMode].Speed);
        jsonWrite(configSetup, "sc", modes[currentMode].Scale);       
    }
    else   {
        #if GENERAL_DEBUG
        LOG.println (F("Не удалось прочитать настройки эффектов из файла"));
        #endif //GENERAL_DEBUG
    }
    file.close();    
    HTTP.send(200, F("text/plain"), F("OK"));
    #if USE_MQTT
    if (espMode == 1U)
    {
      MqttManager::needToPublish = true;
    }
    #endif
}
/*
void handle_alt_panel ()   {
  bool flg = false;
  jsonWrite(configSetup, "alt", HTTP.arg("alt").toInt());
  if (jsonReadtoInt(configSetup, "alt")) flg = FileCopy (F("/main _ctrl_pnl/index1.json.gz") , F("/index.json.gz"));
  else flg = FileCopy (F("/main _ctrl_pnl/index0.json.gz") , F("/index.json.gz"));
  if (flg) {
       HTTP.send(200, F("text/plain"), F("OK"));
     saveConfig();
    }    
  else HTTP.send(404, F("text/plain"), "File not found");
}

void handle_index ()   {
  bool flg = false;
    if (HTTP.arg("index").toInt())
    {
     flg = FileCopy (F("/index/in_final.gz") , F("/index.json.gz"));
    }
    if (flg) HTTP.send(200, F("text/plain"), F("OK"));
    else HTTP.send(404, F("text/plain"), "File not found");
}
*/
void get_time_manual ()   {
    phoneTimeLastSync = HTTP.arg("get_time").toInt() + jsonReadtoInt(configSetup, "timezone") * 3600; // phoneTimeLastSync = tmp + jsonReadtoInt(configSetup, "timezone") * 3600;
    manualTimeShift = phoneTimeLastSync - millis() / 1000UL;
    #ifdef WARNING_IF_NO_TIME
      noTimeClear();
    #endif // WARNING_IF_NO_TIME  
    timeSynched = true;
    getBrightnessForPrintTime();
    #if defined(PHONE_N_MANUAL_TIME_PRIORITY) && defined(USE_NTP) && !USE_RTC
      stillUseNTP = false;
    #endif
    jsonWrite(configSetup, "time", (Get_Time(manualTimeShift+millis()/1000UL)));
    #if USE_RTC
    if (hasRtc) {
    time_t utcTime = localTimeZone.toUTC(manualTimeShift+millis()/1000UL);
    timeToSet.InitWithEpoch32Time(utcTime);
    Rtc.SetDateTime(timeToSet);
    LOG.println(F("Time synced from Browser"));
    }
    #endif
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

#if USE_MP3_PLAYER
void handle_on_sound ()   {
    uint8_t tmp;
    tmp = HTTP.arg("on_sound").toInt();
    jsonWrite(configSetup, "on_sound", tmp);
    if (tmp) {       
        eff_sound_on = eff_volume;
    }
    else {
        eff_sound_on = 0;
    }
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("text/plain"), F("OK"));
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

void handle_volume ()   {
    eff_volume = HTTP.arg("vol").toInt();
    jsonWrite(configSetup, "vol", eff_volume);
    if (mp3_player_connect == 4 && !dawnflag_sound && !sunsetflag_sound && !advert_flag && !weather_advert_flag) {
    send_command(6, FEEDBACK, 0, eff_volume);
    }
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
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

void handle_alarm_on_sound ()   {
    alarm_sound_on = HTTP.arg("on_alm_snd").toInt();
    String configAlarm = readFile(F("config_alarm.json"), 512);
    jsonWrite(configAlarm, "on_alm_snd", alarm_sound_on);
    writeFile(F("config_alarm.json"), configAlarm);
    HTTP.send(200, F("text/plain"), F("OK")); 
}

void handle_sunset_on_sound ()   {
    sunset_sound_on = HTTP.arg("on_sun_snd").toInt();
    String configSunset = readFile(F("config_sunset.json"), 512);
    jsonWrite(configSunset, "on_sun_snd", sunset_sound_on);
    writeFile(F("config_sunset.json"), configSunset);
    HTTP.send(200, F("text/plain"), F("OK")); 
}

void handle_alarm_volume ()   {
    alarm_volume = HTTP.arg("alm_vol").toInt();
    String configAlarm = readFile(F("config_alarm.json"), 512);
    jsonWrite(configAlarm, "alm_vol", alarm_volume);
    writeFile(F("config_alarm.json"), configAlarm);
    if (dawnflag_sound && alarm_sound_on) send_command(6,FEEDBACK,0,alarm_volume); //Громкость
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_sunset_volume ()   {
    sunset_volume = HTTP.arg("sun_vol").toInt();
    String configSunset = readFile(F("config_sunset.json"), 512);
    jsonWrite(configSunset, "sun_vol", sunset_volume);
    writeFile(F("config_sunset.json"), configSunset);
    if (sunsetflag_sound && sunset_sound_on) send_command(6,FEEDBACK,0,sunset_volume); //Громкость
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_day_advert_on_sound ()   {
    day_advert_sound_on = HTTP.arg("on_day_adv").toInt();
    jsonWrite(configSetup, "on_day_adv", day_advert_sound_on);
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_night_advert_on_sound ()   {
    night_advert_sound_on = HTTP.arg("on_night_adv").toInt();
    jsonWrite(configSetup, "on_night_adv", night_advert_sound_on);
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_alarm_advert_sound_on()   {
    alarm_advert_sound_on = HTTP.arg("on_alm_adv").toInt();
    String configAlarm = readFile(F("config_alarm.json"), 512);
    jsonWrite(configAlarm, "on_alm_adv", alarm_advert_sound_on);
    writeFile(F("config_alarm.json"), configAlarm);
    HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_day_weather_advert_on_sound ()   {
    day_weather_advert_sound_on = HTTP.arg("on_day_wadv").toInt();
    jsonWrite(configSetup, "on_day_wadv", day_weather_advert_sound_on);
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_night_weather_advert_on_sound ()   {
    night_weather_advert_sound_on = HTTP.arg("on_night_wadv").toInt();
    jsonWrite(configSetup, "on_night_wadv", night_weather_advert_sound_on);
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_alarm_weather_advert_sound_on()   {
    alarm_weather_advert_sound_on = HTTP.arg("on_alm_wadv").toInt();
    String configAlarm = readFile(F("config_alarm.json"), 512);
    jsonWrite(configAlarm, "on_alm_wadv", alarm_weather_advert_sound_on);
    writeFile(F("config_alarm.json"), configAlarm);
    HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_day_weather_desc_advert_on_sound ()   {
    day_weather_desc_advert_sound_on = HTTP.arg("on_day_wdesc").toInt();
    jsonWrite(configSetup, "on_day_wdesc", day_weather_desc_advert_sound_on);
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_night_weather_desc_advert_on_sound ()   {
    night_weather_desc_advert_sound_on = HTTP.arg("on_night_wdesc").toInt();
    jsonWrite(configSetup, "on_night_wdesc", night_weather_desc_advert_sound_on);
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_alarm_weather_desc_advert_sound_on()   {
    alarm_weather_desc_advert_sound_on = HTTP.arg("on_alm_wdesc").toInt();
    String configAlarm = readFile(F("config_alarm.json"), 512);
    jsonWrite(configAlarm, "on_alm_wdesc", alarm_weather_desc_advert_sound_on);
    writeFile(F("config_alarm.json"), configAlarm);
    HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_day_advert_volume ()   {
    day_advert_volume = HTTP.arg("day_vol").toInt();
    jsonWrite(configSetup, "day_vol", day_advert_volume);
    if ((advert_flag && day_advert_sound_on) || (weather_advert_flag && day_weather_advert_sound_on)) send_command(6,FEEDBACK,0,day_advert_volume); //Громкость времени/погоды
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_night_advert_volume ()   {
    night_advert_volume = HTTP.arg("night_vol").toInt();
    jsonWrite(configSetup, "night_vol", night_advert_volume);
    if ((advert_flag && night_advert_sound_on) || (weather_advert_flag && night_weather_advert_sound_on)) send_command(6,FEEDBACK,0,night_advert_volume); //Громкость времени/погоды
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_sound_set ()   {    // Выбор папок для озвучивания эффектов
    char i[4];
    String configSound = readFile(F("config_sound.json"), 2048); 
    /*#if GENERAL_DEBUG
    LOG.println (F("\nВыбор папок для озвучивания эффектов"));
    LOG.println(configSound);
    #endif*/
    // подготовка  строк с именами полей json file
    esp_task_wdt_reset();
    for (uint8_t k=0; k<MODE_AMOUNT; k++) {
        itoa ((k), i, 10);
        String e = "e" + String (i) ;
        //сохранение параметров в строку
        if (!first_entry)  
           jsonWrite(configSound, e, HTTP.arg(e).toInt());
        //сохранение выбранных папок для озвучивания эффектов
        effects_folders[k] = jsonReadtoInt(configSound, e);
        yield();
    }
    #if GENERAL_DEBUG
    //LOG.println (F("\nВыбор папок для озвучивания эффектов после обработки"));
    //LOG.println(configSound);
    //LOG.print (F("Массив effects_folders [ "));
    esp_task_wdt_reset();
    for (uint8_t k=0; k<MODE_AMOUNT; k++){
        LOG.print (effects_folders[k]);
        LOG.print (F(", "));
        yield();
    }
    LOG.println (F("]"));
    #endif     
    if (!first_entry) {
        writeFile(F("config_sound.json"), configSound );
    }
    HTTP.send(200, F("text/plain"), F("OK"));
}

void handle_folder_down ()   {
    if (true) { //(!pause_on && !mp3_stop && eff_sound_on) {
        CurrentFolder = constrain(CurrentFolder-1, 0, 99);
        jsonWrite(configSetup, "fold_sel", CurrentFolder);
        timeout_save_file_changes = millis();
        bitSet(save_file_changes, 0);
        if (!pause_on && !mp3_stop && eff_sound_on) {
          send_command(0x17,FEEDBACK,0,CurrentFolder);           // Включить непрерывное воспроизведение указанной папки
          delay(mp3_delay);
        }
    }
    /*#if GENERAL_DEBUG
     LOG.print (F("\nCurrent folder "));
     LOG.println (CurrentFolder);
    #endif*/
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
}

void handle_folder_up ()   {
    if (true) { //(!pause_on && !mp3_stop && eff_sound_on) {
        CurrentFolder = constrain(CurrentFolder+1, 0, 99);
        jsonWrite(configSetup, "fold_sel", CurrentFolder);
        timeout_save_file_changes = millis();
        bitSet(save_file_changes, 0);
        if (!pause_on && !mp3_stop && eff_sound_on) {
          send_command(0x17,FEEDBACK,0,CurrentFolder);           // Включить непрерывное воспроизведение указанной папки
          delay(mp3_delay);
        }
    }
    /*#if GENERAL_DEBUG
     LOG.print (F("\nCurrent folder "));
     LOG.println (CurrentFolder);
    #endif*/
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
}

void handle_folder_select()   {
    if (true) { //(!pause_on && !mp3_stop && eff_sound_on) {
        CurrentFolder = HTTP.arg("fold_sel").toInt();          // Выбранная папка
        jsonWrite(configSetup, "fold_sel", CurrentFolder);
        timeout_save_file_changes = millis();
        bitSet(save_file_changes, 0);
        if (!pause_on && !mp3_stop && eff_sound_on) {
          send_command(0x17,FEEDBACK,0,CurrentFolder);           // Включить непрерывное воспроизведение указанной папки
          delay(mp3_delay);
        }
    }
    /*#if GENERAL_DEBUG
     LOG.print (F("\nCurrent folder "));
     LOG.println (CurrentFolder);
    #endif*/
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
    #if USE_MULTIPLE_LAMPS_CONTROL
    repeat_multiple_lamp_control = true;
    #endif  //USE_MULTIPLE_LAMPS_CONTROL
}

void handle_equalizer ()   {
    Equalizer = HTTP.arg("eq").toInt();
    jsonWrite(configSetup, "eq", Equalizer);
    if (mp3_player_connect == 4) send_command(0x07,FEEDBACK,0,Equalizer);  // Эквалайзер
    timeout_save_file_changes = millis();
    bitSet (save_file_changes, 0);
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_alarm_fold_sel ()   {
    AlarmFolder = HTTP.arg("alm_fold").toInt();
    String configAlarm = readFile(F("config_alarm.json"), 512);
    jsonWrite(configAlarm, "alm_fold", AlarmFolder);
    writeFile(F("config_alarm.json"), configAlarm);
    if (alarm_sound_flag) {
        mp3_folder = AlarmFolder;  // Папка будильника
        //mp3_folder_change= 1;
        mp3_folder_last = mp3_folder;
        play_sound();
    }
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_sunset_fold_sel ()   {
    SunsetFolder = HTTP.arg("sun_fold").toInt();
    String configSunset = readFile(F("config_sunset.json"), 512);
    jsonWrite(configSunset, "sun_fold", SunsetFolder);
    writeFile(F("config_sunset.json"), configSunset);
    if (sunset_sound_flag) {
        mp3_folder = SunsetFolder;  // Папка заката
        //mp3_folder_change= 1;
        mp3_folder_last = mp3_folder;
        play_sound();
    }
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_test ()   {
    uint8_t tmp;
    String configHardware = readFile(F("config_hardware.json"), 2048);
    tmp = HTTP.arg("tim_h").toInt();
    ADVERT_TIMER_H = 100 * tmp;
    jsonWrite(configHardware, "tim_h", tmp);
    tmp = HTTP.arg("tim_m").toInt();
    ADVERT_TIMER_M = 100 * tmp;
    jsonWrite(configHardware, "tim_m", tmp);
    tmp = HTTP.arg("delay").toInt();
    mp3_delay = 10 * tmp;
    jsonWrite(configHardware, "delay", tmp);
    writeFile(F("config_hardware.json"), configHardware );
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
    printTime(thisTime, true, ONflag);
    /*#if GENERAL_DEBUG
     LOG.print (F("\nADVERT_TIMER_H = "));
     LOG.println (ADVERT_TIMER_H);
     LOG.print (F("ADVERT_TIMER_M = "));
     LOG.println (ADVERT_TIMER_M);
     LOG.print (F("mp3_delay = "));
     LOG.println (mp3_delay);
    #endif*/
}

void handle_testw ()   {
    uint8_t tmp;
    String configHardware = readFile(F("config_hardware.json"), 2048);
    tmp = HTTP.arg("tim_w").toInt();
    ADVERT_TIMER_W = 100 * tmp;
    jsonWrite(configHardware, "tim_w", tmp);
    String timWdescArg = HTTP.arg("tim_wdesc");
    tmp = timWdescArg.length() ? timWdescArg.toInt() : HTTP.arg("tim_w").toInt();
    ADVERT_TIMER_WDESC = 100 * tmp;
    jsonWrite(configHardware, "tim_wdesc", tmp);
    writeFile(F("config_hardware.json"), configHardware );
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
    printWeather(thisTime, true, ONflag);
    /*#if GENERAL_DEBUG
     LOG.print (F("ADVERT_TIMER_W = "));
     LOG.println (ADVERT_TIMER_W);
     LOG.print (F("ADVERT_TIMER_WDESC = "));
     LOG.println (ADVERT_TIMER_WDESC);
     LOG.print (F("mp3_delay = "));
     LOG.println (mp3_delay);
    #endif*/
}

void handle_mp3_on ()   {
    String configHardware = readFile(F("config_hardware.json"), 2048);
    mp3_player_on = HTTP.arg("mp3_on").toInt();
    jsonWrite(configHardware, "mp3_on", mp3_player_on);
    writeFile(F("config_hardware.json"), configHardware );

    if (!mp3_player_on) {
      if (mp3_player_connect == 4) {
        send_command(0x16,FEEDBACK,0,0);
        delay(mp3_delay);
      }
      mp3_clear_runtime_flags();
      mp3_player_connect = 0;
    }
    else if (mp3_player_connect == 0) {
      mp3_timer = millis();
      mp3_player_connect = 1;
    }

    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

#endif // USE_MP3_PLAYER

void handle_tm1637_on ()   {
    String configHardware = readFile(F("config_hardware.json"), 2048);
    tm1637_on = HTTP.arg("tm1637_on").toInt();
    jsonWrite(configHardware, "tm1637_on", tm1637_on);
    writeFile(F("config_hardware.json"), configHardware );
#if USE_TM1637
    if (!tm1637_on) {
      display.clear();
      DisplayFlag = 0;
    } else {
      display.setBrightness(DispBrightness);
      display.displayByte(_dash, _dash, _dash, _dash);
      LastEffect = 255;
      tmr_clock = millis();
    }
#endif
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_tft_on ()   {
    String configHardware = readFile(F("config_hardware.json"), 2048);
    tft_on = HTTP.arg("tft_on").toInt();
    jsonWrite(configHardware, "tft_on", tft_on);
    writeFile(F("config_hardware.json"), configHardware );
#if USE_TFT
    if (tft_on) {
      TFT_Init();
      tftShowStartText();
    } else {
      TFT_PowerOff();
    }
#endif
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_ir_on ()   {
    String configHardware = readFile(F("config_hardware.json"), 2048);
    ir_on = HTTP.arg("ir_on").toInt();
    jsonWrite(configHardware, "ir_on", ir_on);
    writeFile(F("config_hardware.json"), configHardware );
#if USE_IR_RECEIVER
    if (ir_on) {
      irrecv.enableIRIn();
      IR_Tick_Timer = millis();
      IR_Repeat_Timer = millis();
      lastIRtime = 0;
    } else {
      IR_Tick_Timer = 0;
      IR_Repeat_Timer = 0;
      lastIRtime = 0;
    }
#endif
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_rtc_on ()   {
    String configHardware = readFile(F("config_hardware.json"), 2048);
    rtc_on = HTTP.arg("rtc_on").toInt();
    jsonWrite(configHardware, "rtc_on", rtc_on);
    writeFile(F("config_hardware.json"), configHardware );
#if USE_RTC
    if (rtc_on) {
      hasRtc = true;
      timeSynched = false;
    } else {
      hasRtc = false;
      timeSynched = false;
    }
#endif
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}


void handle_current_limit ()   {
    String configHardware = readFile(F("config_hardware.json"), 1024);
    current_limit = HTTP.arg("cur_lim").toInt();
    //current_limit = constrain (HTTP.arg("cur_lim").toInt(), 0, CURRENT_LIMIT);
    if(current_limit > CURRENT_LIMIT) current_limit = CURRENT_LIMIT;
    jsonWrite(configHardware, "cur_lim", current_limit);
    if(current_limit == 0) current_limit = 0xFFFF;
    FastLED.setMaxPowerInVoltsAndMilliamps(5, current_limit);
    #if GENERAL_DEBUG
    LOG.print (F("\nЛимит тока current_limit = "));
    LOG.println(current_limit);
    #endif
    writeFile(F("config_hardware.json"), configHardware );
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}


void handle_matrix_tipe ()   {
    String configHardware = readFile(F("config_hardware.json"), 1024);
    MATRIX_TYPE = HTTP.arg("m_t").toInt();
    jsonWrite(configHardware, "m_t", MATRIX_TYPE);
    writeFile(F("config_hardware.json"), configHardware );
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_matrix_orientation ()   {
    String configHardware = readFile(F("config_hardware.json"), 1024);    
    ORIENTATION = HTTP.arg("m_o").toInt();
    jsonWrite(configHardware, "m_o", ORIENTATION);
    writeFile(F("config_hardware.json"), configHardware );
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_color_order() {
    String configHardware = readFile(F("config_hardware.json"), 1024);
    colorOrder = HTTP.arg("color_order").toInt();
    jsonWrite(configHardware, "color_order", colorOrder);
    writeFile(F("config_hardware.json"), configHardware);
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_matrix_size ()   {
    String configHardware = readFile(F("config_hardware.json"), 1024);
    uint8_t newWidth = constrain(HTTP.arg("m_w").toInt(), WIDTH_MIN, WIDTH_MAX);
    uint8_t newHeight = constrain(HTTP.arg("m_h").toInt(), HEIGHT_MIN, HEIGHT_MAX);
    matrixWidth = newWidth;
    matrixHeight = newHeight;
    jsonWrite(configHardware, "m_w", matrixWidth);
    jsonWrite(configHardware, "m_h", matrixHeight);
    writeFile(F("config_hardware.json"), configHardware );
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\", \"message\": \"Размер матрицы сохранен. Перезагрузка...\"}"));
    delay(100);
    ESP.restart();
}

void handle_data_lines() {
    String configHardware = readFile(F("config_hardware.json"), 1024);
    uint8_t newDataLines = HTTP.arg("data_lines").toInt();
    ledDataLines = (newDataLines == 1U) ? 1U : 2U;
    jsonWrite(configHardware, "data_lines", ledDataLines);
    writeFile(F("config_hardware.json"), configHardware);
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\", \"message\": \"Настройка DATA-линий сохранена. Перезагрузка...\"}"));
    delay(100);
    ESP.restart();
}

void handle_reset_to_default ()   {
    LOG.println("\n*** Reset to Default ***");
    showWarning(CRGB::Red, 500, 250U);
    esp_task_wdt_reset();
    setModeSettings();
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
    HTTP.send(200, F("text/plain"), F("OK"));
    delay(100);
    ESP.restart();

}

void handle_runing_text_over_effects ()  { //выводить бегущую строку поверх эффектов
    RuninTextOverEffects = HTTP.arg("toe").toInt();
    jsonWrite(configSetup, "toe", RuninTextOverEffects);
    bitSet (save_file_changes, 0);
    timeout_save_file_changes = millis();
    #if USE_MQTT
     if (espMode == 1U)
        {
        MqttManager::needToPublish = true;
     }
    #endif
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_spt ()   {
    SpeedRunningText = HTTP.arg("spt").toInt();
    jsonWrite(configSetup, "spt", SpeedRunningText);
    bitSet (save_file_changes, 0);
    timeout_save_file_changes = millis();
    #if USE_MQTT
     if (espMode == 1U)
        {
        MqttManager::needToPublish = true;
     }
    #endif
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));    
}

void handle_sct ()   {
    ColorRunningText = HTTP.arg("sct").toInt();
    jsonWrite(configSetup, "sct", ColorRunningText);
    bitSet (save_file_changes, 0);
    timeout_save_file_changes = millis();
    #if USE_MQTT
     if (espMode == 1U)
        {
        MqttManager::needToPublish = true;
     }
    #endif
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));    
}

void handle_color_text_fon ()  { //выводить бегущую строку на цветном фоне
    ColorTextFon = HTTP.arg("ctf").toInt();
    jsonWrite(configSetup, "ctf", ColorTextFon);
    bitSet (save_file_changes, 0);
    timeout_save_file_changes = millis();
    #if USE_MQTT
     if (espMode == 1U)
        {
        MqttManager::needToPublish = true;
     }
    #endif
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_use_static_ip() { 
  use_static_ip = HTTP.arg("s_IP").toInt();
  jsonWrite(configSetup, "s_IP", use_static_ip);
  saveConfig();  
  HTTP.send(200, F("text/plain"), F("OK"));
 }

void handle_set_static_ip ()   {
    //uint8_t tmp;
    String configIP = readFile(F("config_ip.json"), 2048);
    jsonWrite(configIP, "ip", HTTP.arg("ip1"));
    jsonWrite(configIP, "gateway", HTTP.arg("gateway"));
    jsonWrite(configIP, "subnet", HTTP.arg("subnet"));
    jsonWrite(configIP, "dns", HTTP.arg("dns"));
    writeFile(F("config_ip.json"), configIP );
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_auto_bri ()   {
    AutoBrightness = HTTP.arg("auto_bri").toInt();
    jsonWrite(configSetup, "auto_bri", AutoBrightness);
    if (ONflag && !dawnFlag) {
        SetBrightness(modes[currentMode].Brightness);  // Переключаем автояркость эффектов
    }
    else if (ONflag && !sunsetFlag) {
        SetBrightness(modes[currentMode].Brightness);  // Переключаем автояркость эффектов
    }
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

#if USE_MQTT

void handle_mqtt_set ()   {
    String configMQTT = readFile(F("config_mqtt.json"), 512);
    String str = HTTP.arg("mq_ip");
    if(!MqttServer.fromString(str)){
        str.toCharArray(MqttHost, str.length()+1);
        mqttIPaddr = false;
    }
    else
        mqttIPaddr = true;
    jsonWrite(configMQTT, "mq_ip", str);
    MqttPort = HTTP.arg("mq_port").toInt();
    jsonWrite(configMQTT, "mq_port", MqttPort);
    str = HTTP.arg("mq_user");
    str.toCharArray(MqttUser, str.length()+1);
    jsonWrite(configMQTT, "mq_user", str);
    str = HTTP.arg("mq_pass");
    str.toCharArray(MqttPassword, str.length()+1);
    jsonWrite(configMQTT, "mq_pass", str);
    str = HTTP.arg("topic");
    str.toCharArray(TopicBase, str.length()+1);
    jsonWrite(configMQTT, "topic", str);
    jsonWrite(configMQTT, "TopicS", (String)MqttManager::clientId+'/'+(String)TopicCmnd); // Выводит в веб-интерфейсе топик подписки лампы
    jsonWrite(configMQTT, "TopicP", (String)MqttManager::clientId+'/'+(String)TopicSnd);  // Выводит в веб-интерфейсе топик публикации лампы
    writeFile(F("config_mqtt.json"), configMQTT );
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
    #if GENERAL_DEBUG
     LOG.print("MQTT server ");
     if(mqttIPaddr)
         LOG.print(MqttServer);
     else
         LOG.print(MqttHost);
     LOG.print(": ");
     LOG.println(MqttPort);
     LOG.print("MQTT User - ");
     LOG.println(MqttUser);
     LOG.print("MQTT Password - ");
     LOG.println(MqttPassword);
     LOG.print("Base Topic - ");
     LOG.println(TopicBase);
     #endif //GENERAL_DEBUG
}

void handle_mqtt_on ()   {
    String configMQTT = readFile(F("config_mqtt.json"), 512);
    MqttOn = HTTP.arg("mq_on").toInt();
    jsonWrite(configMQTT, "mq_on", MqttOn);
    writeFile(F("config_mqtt.json"), configMQTT );
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_mqtt_period ()   {
    String configMQTT = readFile(F("config_mqtt.json"), 512);
    MqttPeriod = HTTP.arg("mq_prd").toInt();
    if (MqttPeriod > 60) MqttPeriod = 60U;
    jsonWrite(configMQTT, "mq_prd", MqttPeriod);
    writeFile(F("config_mqtt.json"), configMQTT );
    HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}
#endif //USE_MQTT

#if USE_BLYNK
void handle_blynk_token() {
  String token = HTTP.arg("blynk_token");
  token.trim();
  jsonWrite(configSetup, "blynk_token", token);
  blynkToken = token;
  saveConfig();
  HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}

void handle_use_blynk() {
  int en = HTTP.arg("use_blynk").toInt();
  if (en != 0) en = 1;
  jsonWrite(configSetup, "use_blynk", en);
  blynkEnabled = (en != 0);
  saveConfig();
  HTTP.send(200, F("application/json"), F("{\"should_refresh\": \"true\"}"));
}
#endif // USE_BLYNK

// Статус кнопки
void handle_button_status() {
  DynamicJsonDocument doc(256);
  String response;

#if USE_BUTTON
  if (button_type) {
    jsonWrite(configSetup, "button_status", "СЕНСОРНАЯ");
    doc["button_status"] = "СЕНСОРНАЯ";
  } else {
    jsonWrite(configSetup, "button_status", "МЕХАНИЧЕСКАЯ");
    doc["button_status"] = "МЕХАНИЧЕСКАЯ";
  }
#else
  jsonWrite(configSetup, "button_status", "ОТКЛЮЧЕНО");
  doc["button_status"] = "ОТКЛЮЧЕНО";
#endif
  serializeJson(doc, response);
  HTTP.send(200, "application/json; charset=utf-8", response);
}

// Статус ИК приемника
void handle_ir_status() {
  DynamicJsonDocument doc(64);
  const char* ir_status;

#if USE_IR_RECEIVER
  if (!ir_on) ir_status = "ОТКЛЮЧЕН В НАСТРОЙКАХ";
  else ir_status = (millis() - lastIRtime < 5000) ? "OK" : "ОЖИДАЕТ КОМАНДУ";
#else
  ir_status = "ОТКЛЮЧЕН";
#endif

  doc["ir"] = ir_status;
  String response;
  serializeJson(doc, response);
  HTTP.send(200, "application/json; charset=utf-8", response);
}

// Статус дисплея
void handle_tm1637_status() {
  DynamicJsonDocument doc(256);
  const char* status;
  bool connected = false;

#if USE_TM1637
  connected = tm1637_on;
  status = tm1637_on ? "ВКЛЮЧЕН" : "ОТКЛЮЧЕН В НАСТРОЙКАХ";
#else
  status = "ОТКЛЮЧЕН";
#endif

  doc["connected"] = connected;
  doc["status"] = status;
  String response;
  serializeJson(doc, response);
  HTTP.send(200, "application/json; charset=utf-8", response);
}

void handle_tft_status() {
  DynamicJsonDocument doc(256);
  const char* status;
  bool connected = false;

#if USE_TFT
  connected = tft_on;
  status = tft_on ? "ВКЛЮЧЕН" : "ОТКЛЮЧЕН В НАСТРОЙКАХ";
#else
  status = "ОТКЛЮЧЕН";
#endif

  doc["connected"] = connected;
  doc["status"] = status;
  String response;
  serializeJson(doc, response);
  HTTP.send(200, "application/json; charset=utf-8", response);
}

// Статус RTC DS3231
void handle_rtc_status() {
  DynamicJsonDocument doc(256);
  const char* status;
  bool connected = false;

#if USE_RTC
  connected = rtc_on && hasRtc;
  status = rtc_on ? (hasRtc ? "ВКЛЮЧЕН" : "НЕТ СВЯЗИ") : "ОТКЛЮЧЕН В НАСТРОЙКАХ";
#else
  status = "ОТКЛЮЧЕН";
#endif

  doc["connected"] = connected;
  doc["status"] = status;
  String response;
  serializeJson(doc, response);
  HTTP.send(200, "application/json; charset=utf-8", response);
}

// Статус плеера
void handle_mp3_status() {
  DynamicJsonDocument doc(512);
  String status;

#if USE_MP3_PLAYER

  if (!mp3_player_on) {
    status = "ОТКЛЮЧЕН В НАСТРОЙКАХ";
  }
  else if (mp3_player_connect == 0) {
    status = "НЕТ СВЯЗИ";
  }
  else if (mp3_player_connect == 5) {
    status = "ПОДКЛЮЧЕН, НЕТ SD/FLASH";
  }
  else if (mp3_player_connect != 4) {
    status = "ИНИЦИАЛИЗАЦИЯ...";
  }
  else if (eff_sound_on && !mp3_stop && !pause_on) {
    status = "ИГРАЕТ";
  }
  else if (eff_sound_on && pause_on) {
    status = "ПАУЗА";
  }
  else if (eff_sound_on && mp3_stop) {
    status = "ВЫКЛЮЧЕН";
  }
  else {
    status = "ГОТОВ";
  }

  doc["enabled"] = mp3_player_on;
  doc["connect"] = mp3_player_connect;
  doc["folder"] = CurrentFolder;
  doc["volume"] = eff_volume;
  doc["sound_on"] = eff_sound_on;
  doc["stop"] = mp3_stop;
  doc["pause"] = pause_on;

#else
  status = "ОТКЛЮЧЕН";

  doc["connect"] = 0;
  doc["folder"] = 0;
  doc["volume"] = 0;
  doc["sound_on"] = 0;
  doc["stop"] = 1;
  doc["pause"] = 1;
#endif

  doc["mp3_status"] = status;

  String response;
  serializeJson(doc, response);
  HTTP.send(200, "application/json; charset=utf-8", response);
}

// Статус Управление несколькими лампами
void handle_multilamp_status() {
  DynamicJsonDocument doc(256);

#if USE_MULTIPLE_LAMPS_CONTROL
  doc["enabled"] = true;
  doc["lamp1"] = ml1;
  doc["lamp2"] = ml2;
  doc["lamp3"] = ml3;
  doc["lamp4"] = ml4;
  doc["lamp5"] = ml5;
  doc["host1"] = String(Host1);
  doc["host2"] = String(Host2);
  doc["host3"] = String(Host3);
  doc["host4"] = String(Host4);
  doc["host5"] = String(Host5);
  #if USE_MP3_PLAYER
  doc["send_sound"] = send_sound;
  doc["send_volume"] = send_eff_volume;
  #endif
#else
  doc["enabled"] = false;
  doc["status"] = "ОТКЛЮЧЕНО";
#endif

  String resp;
  serializeJson(doc, resp);
  HTTP.send(200, "application/json; charset=utf-8", resp);
}

// Статус MQTT
void handle_mqtt_status() {
    DynamicJsonDocument doc(256);
    String status = "ОТКЛЮЧЕНО";
    bool enabled = false;
    bool connected = false;
    bool configEnabled = false;

#if USE_MQTT
    enabled = true;
    String configMQTT = readFile(F("config_mqtt.json"), 512);
    int mq_on = jsonReadtoInt(configMQTT, "mq_on");
    configEnabled = (mq_on != 0);
    if (!configEnabled) {
      status = "НЕ ПОДКЛЮЧЕНО";
      connected = false;
    } else {
      connected = MqttManager::isConnected();
      status = connected ? "ПОДКЛЮЧЕНО" : "НЕ ПОДКЛЮЧЕНО";
    }
#else
    status = "ОТКЛЮЧЕНО";
#endif
    doc["status"]        = status;
    doc["enabled"]       = enabled;
    doc["connected"]     = connected;
    doc["configEnabled"] = configEnabled;
    String resp;
    serializeJson(doc, resp);
    HTTP.send(200, "application/json; charset=utf-8", resp);
}

void handle_blynk_status() {
  DynamicJsonDocument doc(256);

#if !USE_BLYNK
  doc["status"] = "ОТКЛЮЧЕНО";
  doc["fw"] = 0;
  doc["enabled"] = 0;
  doc["token_set"] = 0;
  doc["configured"] = 0;
  doc["connected"] = 0;

#else
  const bool tokenSet = (blynkToken.length() > 0);
  const bool desiredEnabled = blynkEnabled;
  bool configuredMatch = false;
  if (blynkConfigured) {
    configuredMatch = (blynkConfiguredEnabled == desiredEnabled) && (blynkConfiguredToken == blynkToken);
  }
  String status;
  bool connected = false;
  if (!desiredEnabled && !tokenSet) {
    status = "НЕ ПОДКЛЮЧЕНО";
  }
  else if (desiredEnabled && !tokenSet) {
    status = "НЕТ ТОКЕНА";
  }
  else if (desiredEnabled && tokenSet) {
    if (!configuredMatch) {
      status = "ТРЕБУЕТСЯ ПЕРЕЗАГРУЗКА";
    } else {
      connected = Blynk.connected();
      status = connected ? "ПОДКЛЮЧЕНО" : "НЕ ПОДКЛЮЧЕНО";
    }
  }
  else {
    status = "НЕ ПОДКЛЮЧЕНО";
  }

  doc["status"] = status;
  doc["fw"] = 1;
  doc["enabled"] = desiredEnabled ? 1 : 0;
  doc["token_set"] = tokenSet ? 1 : 0;
  doc["configured"] = configuredMatch ? 1 : 0;
  doc["connected"] = connected ? 1 : 0;
#endif

  String resp;
  serializeJson(doc, resp);
  HTTP.send(200, "application/json; charset=utf-8", resp);
}

// Статус OTA
void handle_ota_status() {
  DynamicJsonDocument doc(256);
  const char* ota_status;
  String status;

  #if USE_OTA
    ota_status = "ВКЛЮЧЕНО";
  #else
    ota_status = "ОТКЛЮЧЕНО";
  #endif

  doc["ota"] = ota_status;
  
  String resp;
  serializeJson(doc, resp);
  HTTP.send(200, "application/json; charset=utf-8", resp);
}

// Статус погоды
void handle_show_weather() {
  String configHardware = readFile(F("config_hardware.json"), 2048);
  int enabled = HTTP.arg("show_weather").toInt();
  inClockWeatherMode = (enabled == 1);
  jsonWrite(configHardware, "show_weather", enabled);
  writeFile(F("config_hardware.json"), configHardware);
  if (inClockWeatherMode && WiFi.status() == WL_CONNECTED) {
    weatherUpdateTimer = millis() - WEATHER_UPDATE_INTERVAL + 1000;
  }
  HTTP.send(200, "text/plain", "OK");
}

bool FileCopy (const String& SourceFile , const String& TargetFile)   {
    File S_File = LittleFS.open( SourceFile, "r");
    File T_File = LittleFS.open( TargetFile, "w");
    if (!S_File || !T_File) 
    return false;
    size_t size = S_File.size();
    for (unsigned int i=0; i<size; i++)  {
        T_File.write(S_File.read ());
        esp_task_wdt_reset();
        yield();
    }
    S_File.close();
    T_File.close();
    return true;
}

void EffectList (const String& efflist )   {
    String effList = efflist;
    effList.reserve(17);
    effList += F("ru.ini");
    File R_File = LittleFS.open ( effList, "r" );
    if (!R_File) LOG.println (F("Ошибка. Файл списка эффектов для передачи приложению не найден!"));
    String EffList = R_File.readString();
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.print(EffList.c_str());
    Udp.endPacket();
    #if GENERAL_DEBUG
    LOG.print (F("EffList = "));
    LOG.println (EffList.c_str());
    #endif //GENERAL_DEBUG
    esp_task_wdt_reset();
    yield();
    R_File.close ();
}
 
void SetBrightness(uint8_t brightness)   {
    if (AutoBrightness && !dawnFlag && !day_night) {
        FastLED.setBrightness(constrain(brightness >> AutoBrightness, 1, 100));
    }
    else if (AutoBrightness && !sunsetFlag && !day_night) {
        FastLED.setBrightness(constrain(brightness >> AutoBrightness, 1, 100));
    }
    else
        FastLED.setBrightness(modes[currentMode].Brightness);
}
