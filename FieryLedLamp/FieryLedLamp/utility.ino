// служебные функции

uint16_t matrixFrameDelay(uint16_t baseDelay)
{
  if (baseDelay == 0U) return 0U;

  const uint16_t ledsCount = NUM_LEDS;

  if (ledsCount <= 1024U) return baseDelay;
  if (ledsCount <= 1536U) { uint16_t d = (baseDelay * 3U) / 4U; return d ? d : 1U; }
  if (ledsCount <= 2048U) { uint16_t d = baseDelay / 2U; return d ? d : 1U; }
  if (ledsCount <= 3072U) { uint16_t d = baseDelay / 3U; return d ? d : 1U; }
  { uint16_t d = baseDelay / 4U; return d ? d : 1U; }
}

uint16_t dynamicFrameDelay()
{
  uint16_t baseDelay = 256U - modes[currentMode].Speed;
  return matrixFrameDelay(baseDelay);
}

void effectServiceTick()
{
  static uint16_t serviceCounter = 0;

  if ((++serviceCounter & 0x7FU) != 0U) return;

  #if USE_BUTTON
    if (buttonEnabled) touch.tick();
  #endif

  yield();

  #ifdef ESP32
    esp_task_wdt_reset();
  #endif
}

// залить все
void fillAll(CRGB color)
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = color;
    effectServiceTick();
  }
}

// функция отрисовки точки по координатам X Y
void drawPixelXY(int16_t x, int16_t y, CRGB color)
{
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;

  uint32_t thisPixel = (uint32_t)XY((uint8_t)x, (uint8_t)y) * SEGMENTS;
  if (thisPixel >= NUM_LEDS) return;

  for (uint8_t i = 0; i < SEGMENTS; i++)
  {
    if ((thisPixel + i) < NUM_LEDS) leds[thisPixel + i] = color;
  }
}

// функция получения цвета пикселя по его номеру
uint32_t getPixColor(uint32_t thisSegm)
{
  uint32_t thisPixel = thisSegm * SEGMENTS;
  if (thisPixel > NUM_LEDS - 1) return 0;
  return (((uint32_t)leds[thisPixel].r << 16) | ((uint32_t)leds[thisPixel].g << 8 ) | (uint32_t)leds[thisPixel].b); // а почему не просто return (leds[thisPixel])?
}

// функция получения цвета пикселя в матрице по его координатам
uint32_t getPixColorXY(int16_t x, int16_t y)
{
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return 0;
  return getPixColor(XY((uint8_t)x, (uint8_t)y));
}

// ************* НАСТРОЙКА МАТРИЦЫ *****
/*
#if (CONNECTION_ANGLE == 0 && STRIP_DIRECTION == 0)
#define _WIDTH WIDTH
#define THIS_X x
#define THIS_Y y

#elif (CONNECTION_ANGLE == 0 && STRIP_DIRECTION == 1)
#define _WIDTH HEIGHT
#define THIS_X y
#define THIS_Y x

#elif (CONNECTION_ANGLE == 1 && STRIP_DIRECTION == 0)
#define _WIDTH WIDTH
#define THIS_X x
#define THIS_Y (HEIGHT - y - 1)

#elif (CONNECTION_ANGLE == 1 && STRIP_DIRECTION == 3)
#define _WIDTH HEIGHT
#define THIS_X (HEIGHT - y - 1)
#define THIS_Y x

#elif (CONNECTION_ANGLE == 2 && STRIP_DIRECTION == 2)
#define _WIDTH WIDTH
#define THIS_X (WIDTH - x - 1)
#define THIS_Y (HEIGHT - y - 1)

#elif (CONNECTION_ANGLE == 2 && STRIP_DIRECTION == 3)
#define _WIDTH HEIGHT
#define THIS_X (HEIGHT - y - 1)
#define THIS_Y (WIDTH - x - 1)

#elif (CONNECTION_ANGLE == 3 && STRIP_DIRECTION == 2)
#define _WIDTH WIDTH
#define THIS_X (WIDTH - x - 1)
#define THIS_Y y

#elif (CONNECTION_ANGLE == 3 && STRIP_DIRECTION == 1)
#define _WIDTH HEIGHT
#define THIS_X y
#define THIS_Y (WIDTH - x - 1)

#else
!!!!!!!!!!!!!!!!!!!!!!!!!!!   смотрите инструкцию: https://alexgyver.ru/wp-content/uploads/2018/11/scheme3.jpg
!!!!!!!!!!!!!!!!!!!!!!!!!!!   такого сочетания CONNECTION_ANGLE и STRIP_DIRECTION не бывает
#define _WIDTH WIDTH
#define THIS_X x
#define THIS_Y y
#pragma message "Wrong matrix parameters! Set to default"

#endif
*/
// получить номер пикселя в одной физической матрице по локальным координатам
static uint16_t XY_single(uint8_t x, uint8_t y)
{
  uint8_t THIS_X;
  uint8_t THIS_Y;
  uint8_t _WIDTH = segWidth;

  switch (ORIENTATION)
  {
    case 0: THIS_X = x;                         THIS_Y = y; break;
    case 1: _WIDTH = segHeight;                 THIS_X = y;                         THIS_Y = x; break;
    case 2: THIS_X = x;                         THIS_Y = (segHeight - y - 1U); break;
    case 3: _WIDTH = segHeight;                 THIS_X = (segHeight - y - 1U);      THIS_Y = x; break;
    case 4: THIS_X = (segWidth - x - 1U);       THIS_Y = (segHeight - y - 1U); break;
    case 5: _WIDTH = segHeight;                 THIS_X = (segHeight - y - 1U);      THIS_Y = (segWidth - x - 1U); break;
    case 6: THIS_X = (segWidth - x - 1U);       THIS_Y = y; break;
    case 7: _WIDTH = segHeight;                 THIS_X = y;                         THIS_Y = (segWidth - x - 1U); break;
    default: THIS_X = x;                        THIS_Y = y; break;
  }

  if (!(THIS_Y & 0x01U) || MATRIX_TYPE)
    return (uint16_t)THIS_Y * _WIDTH + THIS_X;
  else
    return (uint16_t)THIS_Y * _WIDTH + _WIDTH - THIS_X - 1U;
}

// получить номер пикселя в общей последовательной цепочке матриц
// m_w/m_h — размер одного модуля, segMatrix_w/segMatrix_h — количество модулей
uint16_t XY(uint8_t x, uint8_t y)
{
  effectServiceTick();

  if (x >= matrixWidth || y >= matrixHeight) return 0;
  if (segWidth == 0U || segHeight == 0U || segMatrixW == 0U || segMatrixH == 0U) return 0;

  if (panelFlip)
  {
    x = matrixWidth - 1U - x;
    y = matrixHeight - 1U - y;
  }

  uint8_t tileX = x / segWidth;
  uint8_t tileY = y / segHeight;
  if (tileX >= segMatrixW || tileY >= segMatrixH) return 0;

  uint8_t localX = x % segWidth;
  uint8_t localY = y % segHeight;
  uint16_t pixelsPerTile = (uint16_t)segWidth * segHeight;
  uint16_t tileNumber = (uint16_t)tileY * segMatrixW + tileX;   // последовательно: слева направо, затем следующая строка
  uint16_t globalPixel = tileNumber * pixelsPerTile + XY_single(localX, localY);

  if (globalPixel >= NUM_LEDS) return 0;
  return globalPixel;
}

// оставлено для совместимости со эффектами из старых прошивок
uint16_t getPixelNumber(uint8_t x, uint8_t y)
{
  return XY(x, y);
}
// ============================================================================
// Текстовый файл настроек эффектов effect.ini
// Формат строки: номер;яркость;скорость;масштаб/цвет;название
// ============================================================================

static uint16_t effectSettingsErrorLine = 0U;
static String effectSettingsErrorReason;

static void clearEffectSettingsError()
{
  effectSettingsErrorLine = 0U;
  effectSettingsErrorReason = "";
}

static void setEffectSettingsError(uint16_t line, const String &reason)
{
  effectSettingsErrorLine = line;
  effectSettingsErrorReason = reason;
}

String getEffectSettingsErrorJson()
{
  String response = F("{\"error\":\"effect.ini\"}");
  if (effectSettingsErrorLine)
    jsonWrite(response, "line", (int)effectSettingsErrorLine);
  if (effectSettingsErrorReason.length())
    jsonWrite(response, "reason", effectSettingsErrorReason);
  return response;
}

static bool parseEffectUnsignedValue(String token, uint16_t minValue,
                                     uint16_t maxValue, uint16_t &result)
{
  token.trim();
  if (token.length() == 0U) return false;

  char *endPtr = nullptr;
  const long value = strtol(token.c_str(), &endPtr, 10);
  if (endPtr == token.c_str() || *endPtr != '\0' ||
      value < minValue || value > maxValue)
  {
    return false;
  }

  result = (uint16_t)value;
  return true;
}
// ============================================================================
// Рабочие параметры эффектов и их сохранение в EEPROM
// ============================================================================
bool effectSettingsDirty = false;
uint32_t effectSettingsChangedAt = 0U;

void syncCurrentEffectToConfig()
{
  if (currentMode >= MODE_AMOUNT) currentMode = 0U;
  jsonWrite(configSetup, "br", modes[currentMode].Brightness);
  jsonWrite(configSetup, "sp", modes[currentMode].Speed);
  jsonWrite(configSetup, "sc", modes[currentMode].Scale);
}

void markEffectSettingsChanged()
{
  effectSettingsDirty = true;
  effectSettingsChangedAt = millis();
}

bool saveEffectSettingsNow(bool force)
{
  if (!effectSettingsDirty && !force) return true;

  if (!EepromManager::EepromPut(modes))
  {
    LOG.println(F("Не удалось сохранить параметры эффектов в EEPROM"));
    return false;
  }

  effectSettingsDirty = false;
  return true;
}

void handleEffectSettingsPersistence()
{
  if (effectSettingsDirty &&
      millis() - effectSettingsChangedAt >= EEPROM_WRITE_DELAY)
  {
    saveEffectSettingsNow(false);
  }
}

bool restoreEffectSettingsForPowerOn()
{
  // Если настройки менялись при выключенной матрице, сначала сохраняем их.
  if (effectSettingsDirty && !saveEffectSettingsNow(false)) return false;

  if (!EepromManager::EepromGet(modes))
  {
    LOG.println(F("Блок параметров эффектов в EEPROM повреждён"));
    return false;
  }

  effectSettingsDirty = false;
  syncCurrentEffectToConfig();
  return true;
}

void persistEffectSettingsBeforePowerOff()
{
  saveEffectSettingsNow(false);
  timeout_save_file_changes = millis() - SAVE_FILE_DELAY_TIMEOUT;
  bitSet(save_file_changes, 0);
  Save_File_Changes();
}

uint8_t effectScaleStepMaximum(uint8_t effectIndex)
{
  if (effectIndex >= MODE_AMOUNT) return 100U;
  return modes[effectIndex].Scale > 100U ? 255U : 100U;
}

void constrainCurrentEffectScale(uint8_t maximum)
{
  if (currentMode >= MODE_AMOUNT || maximum < 1U) return;
  if (modes[currentMode].Scale <= maximum) return;

  modes[currentMode].Scale = maximum;
  syncCurrentEffectToConfig();
  markEffectSettingsChanged();
}

bool applyPendingRandomEffectSettings()
{
  if (!selectedSettings) return true;
  if (currentMode >= MODE_AMOUNT || !ONflag || dawnFlag || sunsetFlag)
  {
    selectedSettings = 0U;
    return false;
  }

  loadingFlag = true;
  effTimer = millis() - 1000UL;
  effectsTick();
  const bool applied = selectedSettings == 0U;
  if (!applied) selectedSettings = 0U;
  else syncCurrentEffectToConfig();
  return applied;
}

static bool parseEffectSettingsLine(String line, uint8_t &effectIndex,
                                    ModeType &settings, String *effectName)
{
  line.trim();
  if (line.length() >= 3U &&
      (uint8_t)line[0] == 0xEFU &&
      (uint8_t)line[1] == 0xBBU &&
      (uint8_t)line[2] == 0xBFU)
  {
    line.remove(0, 3);
    line.trim();
  }
  if (line.length() == 0U || line[0] == '#' || line[0] == ';') return false;

  const int separator1 = line.indexOf(';');
  const int separator2 = separator1 >= 0 ? line.indexOf(';', separator1 + 1) : -1;
  const int separator3 = separator2 >= 0 ? line.indexOf(';', separator2 + 1) : -1;
  const int separator4 = separator3 >= 0 ? line.indexOf(';', separator3 + 1) : -1;
  if (separator1 < 0 || separator2 < 0 || separator3 < 0) return false;

  uint16_t indexValue = 0U;
  uint16_t brightnessValue = 0U;
  uint16_t speedValue = 0U;
  uint16_t scaleValue = 0U;

  const String scaleToken = separator4 >= 0
                          ? line.substring(separator3 + 1, separator4)
                          : line.substring(separator3 + 1);

  if (!parseEffectUnsignedValue(line.substring(0, separator1),
                                0U, MODE_AMOUNT - 1U, indexValue) ||
      !parseEffectUnsignedValue(line.substring(separator1 + 1, separator2),
                                1U, EFFECT_BRIGHTNESS_MAX, brightnessValue) ||
      !parseEffectUnsignedValue(line.substring(separator2 + 1, separator3),
                                1U, 255U, speedValue) ||
      !parseEffectUnsignedValue(scaleToken, 1U, 255U, scaleValue))
  {
    return false;
  }

  effectIndex = (uint8_t)indexValue;
  settings.Brightness = (uint8_t)brightnessValue;
  settings.Speed = (uint8_t)speedValue;
  settings.Scale = (uint8_t)scaleValue;

  if (effectName != nullptr)
  {
    *effectName = separator4 >= 0 ? line.substring(separator4 + 1) : String();
    effectName->trim();
  }
  return true;
}

static const uint8_t EFFECT_MERGE_BRIGHTNESS = 0x01U;
static const uint8_t EFFECT_MERGE_SPEED      = 0x02U;
static const uint8_t EFFECT_MERGE_SCALE      = 0x04U;
static const uint8_t EFFECT_MERGE_SEEN       = 0x40U;
static const uint8_t EFFECT_MERGE_DUPLICATE  = 0x80U;

// Старый effect.ini может быть неполным или частично повреждённым. Для
// обновления разбираем каждое числовое поле отдельно: корректные значения
// сохраняются, отсутствующие и неверные затем дополняются из нового файла.
static bool parseEffectSettingsMergeLine(String line, uint16_t &effectIndex,
                                         ModeType &settings, uint8_t &validMask)
{
  line.trim();
  if (line.length() >= 3U &&
      (uint8_t)line[0] == 0xEFU &&
      (uint8_t)line[1] == 0xBBU &&
      (uint8_t)line[2] == 0xBFU)
  {
    line.remove(0, 3);
    line.trim();
  }
  if (line.length() == 0U || line[0] == '#' || line[0] == ';') return false;

  const int separator1 = line.indexOf(';');
  if (separator1 < 0) return false;
  const String indexToken = line.substring(0, separator1);
  if (!parseEffectUnsignedValue(indexToken, 0U, 65535U, effectIndex)) return false;

  validMask = 0U;
  const int separator2 = line.indexOf(';', separator1 + 1);
  const int separator3 = separator2 >= 0 ? line.indexOf(';', separator2 + 1) : -1;
  const int separator4 = separator3 >= 0 ? line.indexOf(';', separator3 + 1) : -1;
  uint16_t value = 0U;

  const String brightnessToken = separator2 >= 0
                               ? line.substring(separator1 + 1, separator2)
                               : line.substring(separator1 + 1);
  if (parseEffectUnsignedValue(brightnessToken, 1U,
                               EFFECT_BRIGHTNESS_MAX, value))
  {
    settings.Brightness = (uint8_t)value;
    validMask |= EFFECT_MERGE_BRIGHTNESS;
  }

  if (separator2 >= 0)
  {
    const String speedToken = separator3 >= 0
                            ? line.substring(separator2 + 1, separator3)
                            : line.substring(separator2 + 1);
    if (parseEffectUnsignedValue(speedToken, 1U, 255U, value))
    {
      settings.Speed = (uint8_t)value;
      validMask |= EFFECT_MERGE_SPEED;
    }
  }

  if (separator3 >= 0)
  {
    const String scaleToken = separator4 >= 0
                            ? line.substring(separator3 + 1, separator4)
                            : line.substring(separator3 + 1);
    if (parseEffectUnsignedValue(scaleToken, 1U, 255U, value))
    {
      settings.Scale = (uint8_t)value;
      validMask |= EFFECT_MERGE_SCALE;
    }
  }

  return true;
}

static bool readEffectSettingsFileAtPath(const char *fileName,
                                         ModeType destination[])
{
  File file = LittleFS.open(fileName, "r");
  if (!file)
  {
    setEffectSettingsError(0U, F("не удалось открыть файл"));
    return false;
  }

  ModeType parsedSettings[MODE_AMOUNT];
  bool found[MODE_AMOUNT] = { false };
  uint16_t foundCount = 0U;
  uint16_t lineNumber = 0U;

  while (file.available())
  {
    String line = file.readStringUntil('\n');
    lineNumber++;
    String checkedLine = line;
    checkedLine.trim();
    if (checkedLine.length() >= 3U &&
        (uint8_t)checkedLine[0] == 0xEFU &&
        (uint8_t)checkedLine[1] == 0xBBU &&
        (uint8_t)checkedLine[2] == 0xBFU)
    {
      checkedLine.remove(0, 3);
      checkedLine.trim();
    }
    if (checkedLine.length() == 0U ||
        checkedLine[0] == '#' || checkedLine[0] == ';') continue;

    uint8_t effectIndex = 0U;
    ModeType settings;
    if (!parseEffectSettingsLine(line, effectIndex, settings, nullptr))
    {
      String reason = F("формат: номер 0..");
      reason += MODE_AMOUNT - 1U;
      reason += F("; яркость 1..100; скорость и масштаб 1..255");
      setEffectSettingsError(lineNumber, reason);
      file.close();
      return false;
    }
    if (found[effectIndex])
    {
      String reason = F("повтор номера эффекта ");
      reason += effectIndex;
      setEffectSettingsError(lineNumber, reason);
      file.close();
      return false;
    }

    parsedSettings[effectIndex] = settings;
    found[effectIndex] = true;
    foundCount++;
    yield();
  }
  file.close();

  if (foundCount != MODE_AMOUNT)
  {
    String reason = F("найдено ");
    reason += foundCount;
    reason += F(" из ");
    reason += MODE_AMOUNT;
    reason += F(" эффектов");
    setEffectSettingsError(0U, reason);
    return false;
  }

  memcpy(destination, parsedSettings, sizeof(parsedSettings));
  clearEffectSettingsError();
  return true;
}

bool readEffectSettingsFromFile(ModeType destination[])
{
  if (LittleFS.exists(EFFECT_SETTINGS_FILE))
    return readEffectSettingsFileAtPath(EFFECT_SETTINGS_FILE, destination);

  ModeType recovered[MODE_AMOUNT];
  if (LittleFS.exists(EFFECT_SETTINGS_BACKUP_FILE) &&
      readEffectSettingsFileAtPath(EFFECT_SETTINGS_BACKUP_FILE, recovered))
  {
    LittleFS.rename(EFFECT_SETTINGS_BACKUP_FILE, EFFECT_SETTINGS_FILE);
    memcpy(destination, recovered, sizeof(recovered));
    clearEffectSettingsError();
    return true;
  }

  if (LittleFS.exists(EFFECT_SETTINGS_TEMP_FILE) &&
      readEffectSettingsFileAtPath(EFFECT_SETTINGS_TEMP_FILE, recovered))
  {
    LittleFS.rename(EFFECT_SETTINGS_TEMP_FILE, EFFECT_SETTINGS_FILE);
    memcpy(destination, recovered, sizeof(recovered));
    clearEffectSettingsError();
    return true;
  }

  setEffectSettingsError(0U, F("файл /effect.ini отсутствует"));
  return false;
}

bool resetAllEffectsToDefaults()
{
  ModeType parsedDefaults[MODE_AMOUNT];
  if (!readEffectSettingsFromFile(parsedDefaults)) return false;

  memcpy(modes, parsedDefaults, sizeof(parsedDefaults));
  selectedSettings = 0U;
  syncCurrentEffectToConfig();
  markEffectSettingsChanged();
  if (!saveEffectSettingsNow(false)) return false;
  loadingFlag = true;
  SetBrightness(modes[currentMode].Brightness);
  return true;
}

static bool writeEffectSettingsHeader(File &file)
{
  bool success = true;
  success = (file.println(F("# Формат: номер;яркость;скорость;масштаб/цвет;название")) > 0U) && success;
  return success;
}

static bool writeEffectSettingsRecord(File &file, uint8_t effectIndex,
                                      const ModeType &settings, String effectName)
{
  effectName.trim();
  if (effectName.length() == 0U)
  {
    effectName = F("Эффект ");
    effectName += effectIndex;
  }
  effectName.replace("\r", " ");
  effectName.replace("\n", " ");

  bool success = true;
  success = (file.print(effectIndex) > 0U) && success;
  success = (file.print(';') > 0U) && success;
  success = (file.print(constrain(settings.Brightness, 1, EFFECT_BRIGHTNESS_MAX)) > 0U) && success;
  success = (file.print(';') > 0U) && success;
  success = (file.print(constrain(settings.Speed, 1, 255)) > 0U) && success;
  success = (file.print(';') > 0U) && success;
  success = (file.print(constrain(settings.Scale, 1, 255)) > 0U) && success;
  success = (file.print(';') > 0U) && success;
  success = (file.println(effectName) > 0U) && success;
  return success;
}

bool mergeEffectSettingsForRestore(const char* savedFileName,
                                   const char* newFileName,
                                   bool useEepromFallback)
{
  if (!savedFileName || !newFileName ||
      strcmp(savedFileName, newFileName) == 0)
  {
    setEffectSettingsError(0U, F("неверные пути слияния effect.ini"));
    return false;
  }

  ModeType newSettings[MODE_AMOUNT];
  if (!readEffectSettingsFileAtPath(newFileName, newSettings))
  {
    LOG.println(F("Новый /effect.ini не прошёл полную проверку"));
    return false;
  }

  ModeType savedSettings[MODE_AMOUNT];
  uint8_t savedState[MODE_AMOUNT] = { 0U };
  uint16_t savedLines = 0U;
  uint16_t removedLines = 0U;
  uint16_t duplicateLines = 0U;
  bool usedEeprom = false;
  bool usedLegacyBinary = false;

  if (LittleFS.exists(savedFileName))
  {
    File savedFile = LittleFS.open(savedFileName, "r");
    if (!savedFile)
    {
      setEffectSettingsError(0U, F("не удалось открыть сохранённый effect.ini"));
      return false;
    }

    const size_t savedFileSize = savedFile.size();
    uint16_t parsedTextLines = 0U;
    while (savedFile.available())
    {
      String line = savedFile.readStringUntil('\n');
      uint16_t effectIndex = 0U;
      ModeType settings;
      uint8_t validMask = 0U;
      if (!parseEffectSettingsMergeLine(line, effectIndex, settings, validMask))
      {
        yield();
        continue;
      }

      parsedTextLines++;
      if (effectIndex >= MODE_AMOUNT)
      {
        removedLines++;
        yield();
        continue;
      }

      if ((savedState[effectIndex] & EFFECT_MERGE_SEEN) != 0U)
      {
        savedState[effectIndex] = EFFECT_MERGE_SEEN | EFFECT_MERGE_DUPLICATE;
        duplicateLines++;
        yield();
        continue;
      }

      savedSettings[effectIndex] = settings;
      savedState[effectIndex] = EFFECT_MERGE_SEEN | validMask;
      savedLines++;
      yield();
    }
    savedFile.close();

    if (parsedTextLines == 0U && savedFileSize > 0U &&
        savedFileSize <= (size_t)EEPROM_MODE_STRUCT_SIZE * 255U &&
        savedFileSize % EEPROM_MODE_STRUCT_SIZE == 0U)
    {
      File legacyFile = LittleFS.open(savedFileName, "r");
      if (!legacyFile)
      {
        setEffectSettingsError(0U, F("не удалось открыть старый бинарный effect.ini"));
        return false;
      }

      const size_t legacyCount = savedFileSize / EEPROM_MODE_STRUCT_SIZE;
      for (size_t i = 0U; i < legacyCount; i++)
      {
        const int brightness = legacyFile.read();
        const int speedValue = legacyFile.read();
        const int scaleValue = legacyFile.read();
        if (brightness < 0 || speedValue < 0 || scaleValue < 0)
        {
          legacyFile.close();
          setEffectSettingsError(0U, F("повреждён старый бинарный effect.ini"));
          return false;
        }

        if (i >= MODE_AMOUNT)
        {
          removedLines++;
          continue;
        }

        uint8_t validMask = 0U;
        if (brightness >= 1 && brightness <= EFFECT_BRIGHTNESS_MAX)
        {
          savedSettings[i].Brightness = (uint8_t)brightness;
          validMask |= EFFECT_MERGE_BRIGHTNESS;
        }
        if (speedValue >= 1 && speedValue <= 255)
        {
          savedSettings[i].Speed = (uint8_t)speedValue;
          validMask |= EFFECT_MERGE_SPEED;
        }
        if (scaleValue >= 1 && scaleValue <= 255)
        {
          savedSettings[i].Scale = (uint8_t)scaleValue;
          validMask |= EFFECT_MERGE_SCALE;
        }
        savedState[i] = EFFECT_MERGE_SEEN | validMask;
        savedLines++;
      }
      legacyFile.close();
      usedLegacyBinary = true;
    }
  }
  else if (useEepromFallback && EepromManager::EepromGet(savedSettings))
  {
    for (uint8_t i = 0U; i < MODE_AMOUNT; i++)
    {
      savedState[i] = EFFECT_MERGE_SEEN |
                      EFFECT_MERGE_BRIGHTNESS |
                      EFFECT_MERGE_SPEED |
                      EFFECT_MERGE_SCALE;
    }
    savedLines = MODE_AMOUNT;
    usedEeprom = true;
  }

  File newFile = LittleFS.open(newFileName, "r");
  if (!newFile)
  {
    setEffectSettingsError(0U, F("не удалось повторно открыть новый effect.ini"));
    return false;
  }

  LittleFS.remove(savedFileName);
  File mergedFile = LittleFS.open(savedFileName, "w");
  if (!mergedFile)
  {
    newFile.close();
    setEffectSettingsError(0U, F("не удалось создать объединённый effect.ini"));
    return false;
  }

  bool writeSuccess = writeEffectSettingsHeader(mergedFile);
  bool written[MODE_AMOUNT] = { false };
  uint16_t writtenCount = 0U;
  uint16_t restoredValues = 0U;

  while (newFile.available())
  {
    String line = newFile.readStringUntil('\n');
    uint8_t effectIndex = 0U;
    ModeType ignoredSettings;
    String effectName;
    if (!parseEffectSettingsLine(line, effectIndex, ignoredSettings, &effectName))
    {
      yield();
      continue;
    }

    ModeType mergedSettings = newSettings[effectIndex];
    const uint8_t state = savedState[effectIndex];
    if ((state & EFFECT_MERGE_DUPLICATE) == 0U)
    {
      if ((state & EFFECT_MERGE_BRIGHTNESS) != 0U)
      {
        mergedSettings.Brightness = savedSettings[effectIndex].Brightness;
        restoredValues++;
      }
      if ((state & EFFECT_MERGE_SPEED) != 0U)
      {
        mergedSettings.Speed = savedSettings[effectIndex].Speed;
        restoredValues++;
      }
      if ((state & EFFECT_MERGE_SCALE) != 0U)
      {
        mergedSettings.Scale = savedSettings[effectIndex].Scale;
        restoredValues++;
      }
    }

    writeSuccess = writeEffectSettingsRecord(mergedFile, effectIndex,
                                             mergedSettings, effectName) &&
                   writeSuccess;
    written[effectIndex] = true;
    writtenCount++;
    yield();
  }

  newFile.close();
  mergedFile.flush();
  mergedFile.close();

  if (!writeSuccess || writtenCount != MODE_AMOUNT)
  {
    LittleFS.remove(savedFileName);
    setEffectSettingsError(0U, F("неполный объединённый effect.ini"));
    return false;
  }

  for (uint8_t i = 0U; i < MODE_AMOUNT; i++)
  {
    if (!written[i])
    {
      LittleFS.remove(savedFileName);
      setEffectSettingsError(0U, F("в объединённом effect.ini пропущен эффект"));
      return false;
    }
  }

  ModeType verifiedSettings[MODE_AMOUNT];
  if (!readEffectSettingsFileAtPath(savedFileName, verifiedSettings))
  {
    LittleFS.remove(savedFileName);
    LOG.println(F("Объединённый /effect.ini не прошёл контрольную проверку"));
    return false;
  }

  LOG.printf_P(PSTR("effect.ini: строк старого=%u, сохранено значений=%u, удалено строк=%u, дубликатов=%u%s\n"),
               savedLines, restoredValues, removedLines, duplicateLines,
               usedEeprom ? ", источник=EEPROM" :
               (usedLegacyBinary ? ", старый формат=бинарный" : ""));
  clearEffectSettingsError();
  return true;
}

bool writeEffectSettingsToFile(const ModeType source[])
{
  clearEffectSettingsError();
  LittleFS.remove(EFFECT_SETTINGS_TEMP_FILE);
  File tempFile = LittleFS.open(EFFECT_SETTINGS_TEMP_FILE, "w");
  if (!tempFile)
  {
    setEffectSettingsError(0U, F("не удалось создать /effect.tmp"));
    LOG.println(F("Не удалось создать /effect.tmp"));
    return false;
  }

  bool writeSuccess = writeEffectSettingsHeader(tempFile);

  bool written[MODE_AMOUNT] = { false };
  uint16_t writtenCount = 0U;
  File oldFile = LittleFS.open(EFFECT_SETTINGS_FILE, "r");

  if (oldFile)
  {
    while (oldFile.available())
    {
      String line = oldFile.readStringUntil('\n');
      uint8_t effectIndex = 0U;
      ModeType ignoredSettings;
      String effectName;
      if (parseEffectSettingsLine(line, effectIndex, ignoredSettings, &effectName) &&
          !written[effectIndex])
      {
        writeSuccess = writeEffectSettingsRecord(tempFile, effectIndex,
                                                 source[effectIndex], effectName) &&
                       writeSuccess;
        written[effectIndex] = true;
        writtenCount++;
      }
      yield();
    }
    oldFile.close();
  }

  // Если строка была удалена вручную, всё равно создаём запись для этого эффекта.
  for (uint8_t i = 0U; i < MODE_AMOUNT; i++)
  {
    if (!written[i])
    {
      writeSuccess = writeEffectSettingsRecord(tempFile, i, source[i], String()) &&
                     writeSuccess;
      writtenCount++;
    }
    yield();
  }

  tempFile.flush();
  tempFile.close();
  if (!writeSuccess || writtenCount != MODE_AMOUNT)
  {
    LittleFS.remove(EFFECT_SETTINGS_TEMP_FILE);
    setEffectSettingsError(0U, F("ошибка записи /effect.tmp"));
    LOG.println(F("Ошибка записи /effect.tmp"));
    return false;
  }

  ModeType verifiedSettings[MODE_AMOUNT];
  if (!readEffectSettingsFileAtPath(EFFECT_SETTINGS_TEMP_FILE, verifiedSettings))
  {
    LittleFS.remove(EFFECT_SETTINGS_TEMP_FILE);
    LOG.println(F("Проверка /effect.tmp завершилась ошибкой"));
    return false;
  }

  LittleFS.remove(EFFECT_SETTINGS_BACKUP_FILE);
  const bool hadOldFile = LittleFS.exists(EFFECT_SETTINGS_FILE);
  if (hadOldFile &&
      !LittleFS.rename(EFFECT_SETTINGS_FILE, EFFECT_SETTINGS_BACKUP_FILE))
  {
    LittleFS.remove(EFFECT_SETTINGS_TEMP_FILE);
    setEffectSettingsError(0U, F("не удалось создать резервную копию"));
    LOG.println(F("Не удалось подготовить замену /effect.ini"));
    return false;
  }

  if (!LittleFS.rename(EFFECT_SETTINGS_TEMP_FILE, EFFECT_SETTINGS_FILE))
  {
    if (hadOldFile)
      LittleFS.rename(EFFECT_SETTINGS_BACKUP_FILE, EFFECT_SETTINGS_FILE);
    LittleFS.remove(EFFECT_SETTINGS_TEMP_FILE);
    setEffectSettingsError(0U, F("не удалось заменить /effect.ini"));
    LOG.println(F("Не удалось заменить /effect.ini"));
    return false;
  }

  clearEffectSettingsError();
  return true;
}

// неточный, зато более быстрый квадратный корень
float sqrt3(const float x)
{
  union
  {
    int i;
    float x;
  } u;

  u.x = x;
  u.i = (1<<29) + (u.i >> 1) - (1<<22);
  return u.x;
}

uint8_t SpeedFactor(uint8_t spd) {
  uint16_t result = ((uint32_t)spd * NUM_LEDS) / 1024UL;
  if (result > 255U) result = 255U;
#if GENERAL_DEBUG
  LOG.printf_P(PSTR("Speed Factor • %03d\n\r"), (uint8_t)result);
#endif
  return (uint8_t)result;
}
