#pragma once

/*
 * Схема использования EEPROM памяти:
 * Используются адреса:
 * Начало    Длина    Описание
 * 0-63      64       Пароль wi-fi вашего роутера
 * 64        1        Сигнатура блока параметров эффектов
 * 65        1        Версия формата
 * 66        1        Количество эффектов
 * 67        1        Резерв
 * 68-71     4        CRC32 массива modes[]
 * 72-79     8        Резерв
 * ***************** массив modes (эффекты)
 * 80-82     3        режим №1:  яркость, скорость, масштаб (по одному байту)
 * 83-85     3        режим №2:  яркость, скорость, масштаб (по одному байту)
 * 86-88     3        режим №3:  яркость, скорость, масштаб (по одному байту)
 * ...
*/


static_assert(sizeof(ModeType) == EEPROM_MODE_STRUCT_SIZE,
              "EEPROM layout must match ModeType");

class EepromManager
{
  public:
    static bool InitEepromSettings(ModeType destination[],
                                   const ModeType defaults[],
                                   bool defaultsValid)
    {
      EEPROM.begin(EEPROM_TOTAL_BYTES_USED);
      delay(50);

      if (EepromGet(destination)) return true;
      if (MigrateLegacySettings(destination)) return true;
      if (defaultsValid)
      {
        memcpy(destination, defaults, sizeof(ModeType) * MODE_AMOUNT);
        return EepromPut(destination);
      }

      FillSafeSettings(destination);
      return false;
    }

    static bool EepromGet(ModeType destination[]) {
      if (EEPROM.read(EEPROM_EFFECTS_MAGIC_ADDRESS) != EEPROM_EFFECTS_MAGIC ||
          EEPROM.read(EEPROM_EFFECTS_VERSION_ADDRESS) != EEPROM_EFFECTS_LAYOUT_VERSION ||
          EEPROM.read(EEPROM_EFFECTS_COUNT_ADDRESS) != (uint8_t)MODE_AMOUNT)
      {
        return false;
      }

      ModeType loaded[MODE_AMOUNT];
      for (uint8_t i = 0; i < MODE_AMOUNT; i++)
      {
        EEPROM.get(EEPROM_MODES_START_ADDRESS + EEPROM_MODE_STRUCT_SIZE * i,
                   loaded[i]);
      }

      uint32_t storedCrc = 0U;
      EEPROM.get(EEPROM_EFFECTS_CRC_ADDRESS, storedCrc);
      if (storedCrc != EffectsCrc(loaded) || !SettingsAreValid(loaded))
      {
        return false;
      }

      memcpy(destination, loaded, sizeof(loaded));
      return true;
    }

    static bool EepromPut(ModeType source[]) {
      for (uint8_t i = 0; i < MODE_AMOUNT; i++)
      {
        NormalizeMode(source[i]);
        EEPROM.put(EEPROM_MODES_START_ADDRESS + EEPROM_MODE_STRUCT_SIZE * i,
                   source[i]);
      }

      const uint32_t crc = EffectsCrc(source);
      EEPROM.put(EEPROM_EFFECTS_CRC_ADDRESS, crc);
      EEPROM.write(EEPROM_EFFECTS_COUNT_ADDRESS, (uint8_t)MODE_AMOUNT);
      EEPROM.write(EEPROM_EFFECTS_VERSION_ADDRESS, EEPROM_EFFECTS_LAYOUT_VERSION);
      EEPROM.write(EEPROM_EFFECTS_MAGIC_ADDRESS, EEPROM_EFFECTS_MAGIC);

      return EEPROM.commit();
    }

    static void NormalizeMode(ModeType &mode) {
      mode.Brightness = constrain(mode.Brightness, 1, EFFECT_BRIGHTNESS_MAX);
      if (mode.Speed == 0U) mode.Speed = 1U;
      if (mode.Scale == 0U) mode.Scale = 1U;
    }

    static void FillSafeSettings(ModeType destination[]) {
      for (uint8_t i = 0; i < MODE_AMOUNT; i++)
      {
        destination[i].Brightness = 10U;
        destination[i].Speed = 128U;
        destination[i].Scale = 50U;
      }
    }

    static uint32_t EffectsCrc(const ModeType source[]) {
      const uint8_t* data = reinterpret_cast<const uint8_t*>(source);
      size_t len = sizeof(ModeType) * MODE_AMOUNT;
      uint32_t crc = 0xFFFFFFFFUL;
      while (len--)
      {
        crc ^= *data++;
        for (uint8_t i = 0; i < 8U; i++)
          crc = (crc & 1U) ? ((crc >> 1) ^ 0xEDB88320UL) : (crc >> 1);
      }
      return ~crc;
    }

    static bool SettingsAreValid(const ModeType source[]) {
      for (uint8_t i = 0; i < MODE_AMOUNT; i++)
      {
        if (source[i].Brightness < 1U ||
            source[i].Brightness > EFFECT_BRIGHTNESS_MAX ||
            source[i].Speed < 1U || source[i].Scale < 1U)
        {
          return false;
        }
      }
      return true;
    }

    static bool MigrateLegacySettings(ModeType destination[]) {
      if (EEPROM.read(EEPROM_EFFECTS_MAGIC_ADDRESS) != EEPROM_LEGACY_FIRST_RUN_MARK)
        return false;

      ModeType legacy[MODE_AMOUNT];
      for (uint8_t i = 0; i < MODE_AMOUNT; i++)
      {
        EEPROM.get(EEPROM_MODES_START_ADDRESS + EEPROM_MODE_STRUCT_SIZE * i,
                   legacy[i]);
      }
      if (!SettingsAreValid(legacy)) return false;

      memcpy(destination, legacy, sizeof(legacy));
      return EepromPut(destination);
    }

    struct WifiBackupData {
      uint32_t magic;
      uint8_t version;
      uint8_t pending;
      uint8_t espMode;
      uint16_t timeoutSec;
      char ssid1[33];
      char password1[65];
      uint32_t crc32;
    };

    static uint32_t WifiBackupCrc(const uint8_t* data, size_t len) {
      uint32_t crc = 0xFFFFFFFFUL;
      while (len--) {
        crc ^= *data++;
        for (uint8_t i = 0; i < 8; i++) {
          crc = (crc & 1U) ? ((crc >> 1) ^ 0xEDB88320UL) : (crc >> 1);
        }
      }
      return ~crc;
    }

    static void WifiBackupCopy(char* dst, size_t dstSize, const String& src) {
      if (!dst || dstSize == 0) return;
      memset(dst, 0, dstSize);
      if (src.length() == 0) return;
      strncpy(dst, src.c_str(), dstSize - 1);
      dst[dstSize - 1] = 0;
    }

    static bool SaveWifiBackupForGitHubOta(String& configSetup) {
      if (configSetup.length() == 0) return false;

      WifiBackupData data;
      memset(&data, 0, sizeof(data));
      data.magic = EEPROM_WIFI_BACKUP_MAGIC;
      data.version = EEPROM_WIFI_BACKUP_VERSION;
      data.pending = EEPROM_WIFI_BACKUP_PENDING_MARK;
      data.espMode = (uint8_t)jsonReadtoInt(configSetup, "ESP_mode");
      data.timeoutSec = (uint16_t)jsonReadtoInt(configSetup, "TimeOut");
      WifiBackupCopy(data.ssid1, sizeof(data.ssid1), jsonRead(configSetup, "ssid"));
      WifiBackupCopy(data.password1, sizeof(data.password1), jsonRead(configSetup, "password"));
      data.crc32 = WifiBackupCrc((const uint8_t*)&data, sizeof(data) - sizeof(data.crc32));

      if (sizeof(data) > EEPROM_WIFI_BACKUP_SIZE) return false;
      const uint8_t* p = (const uint8_t*)&data;
      for (uint16_t i = 0; i < sizeof(data); i++) {
        EEPROM.write(EEPROM_WIFI_BACKUP_START_ADDRESS + i, p[i]);
      }
      for (uint16_t i = sizeof(data); i < EEPROM_WIFI_BACKUP_SIZE; i++) {
        EEPROM.write(EEPROM_WIFI_BACKUP_START_ADDRESS + i, 0);
      }
      return EEPROM.commit();
    }

    static bool ReadWifiBackupRaw(WifiBackupData &data) {
      memset(&data, 0, sizeof(data));
      if (sizeof(data) > EEPROM_WIFI_BACKUP_SIZE) return false;
      uint8_t* p = (uint8_t*)&data;
      for (uint16_t i = 0; i < sizeof(data); i++) {
        p[i] = EEPROM.read(EEPROM_WIFI_BACKUP_START_ADDRESS + i);
      }
      if (data.magic != EEPROM_WIFI_BACKUP_MAGIC) return false;
      if (data.version != EEPROM_WIFI_BACKUP_VERSION) return false;
      uint32_t crc = WifiBackupCrc((const uint8_t*)&data, sizeof(data) - sizeof(data.crc32));
      if (crc != data.crc32) return false;
      return true;
    }

    static bool IsGitHubOtaRestorePending() {
      WifiBackupData data;
      return ReadWifiBackupRaw(data) &&
             data.pending == EEPROM_WIFI_BACKUP_PENDING_MARK;
    }

    static void ClearWifiBackupPending() {
      WifiBackupData data;
      if (!ReadWifiBackupRaw(data)) return;
      data.pending = 0;
      data.crc32 = WifiBackupCrc((const uint8_t*)&data, sizeof(data) - sizeof(data.crc32));
      const uint8_t* p = (const uint8_t*)&data;
      for (uint16_t i = 0; i < sizeof(data); i++) {
        EEPROM.write(EEPROM_WIFI_BACKUP_START_ADDRESS + i, p[i]);
      }
      EEPROM.commit();
    }

    static bool RestoreWifiBackupAfterGitHubOta(String &configSetup) {
      WifiBackupData data;
      if (!ReadWifiBackupRaw(data)) return false;
      if (data.pending != EEPROM_WIFI_BACKUP_PENDING_MARK) return false;

      if (configSetup == F("Failed") || configSetup == F("Large") || configSetup.length() == 0) {
        configSetup = F("{}");
      }

      jsonWrite(configSetup, "ESP_mode", (int)data.espMode);
      jsonWrite(configSetup, "TimeOut", (int)data.timeoutSec);
      jsonWrite(configSetup, "ssid", String(data.ssid1));
      jsonWrite(configSetup, "password", String(data.password1));

      ClearWifiBackupPending();
      return true;
    }

  private:
};
