#pragma once

/*
 * Схема использования EEPROM памяти:
 * Используются адреса:
 * Начало    Длина    Описание
 * 0-63      64       Пароль wi-fi вашего роутера
 * 64        1        Признак первого запуска
 * 65-79     15       Специальные настройки
 * ***************** массив modes (эффекты)
 * 80-82     3        режим №1:  яркость, скорость, масштаб (по одному байту)
 * 83-85     3        режим №2:  яркость, скорость, масштаб (по одному байту)
 * 86-88     3        режим №3:  яркость, скорость, масштаб (по одному байту)
 * ...
*/


class EepromManager
{
  public:
    static void InitEepromSettings(ModeType modes[], void (*restoreDefaultSettings)())
    {
      EEPROM.begin(EEPROM_TOTAL_BYTES_USED);
      delay(50);

      // записываем в EEPROM начальное состояние настроек, если их там ещё нет
      if (EEPROM.read(EEPROM_FIRST_RUN_ADDRESS) != EEPROM_FIRST_RUN_MARK)
      {
        restoreDefaultSettings(); // а почему бы нам не восстановить настройки по умолчанию в этом месте?

        EEPROM.write(EEPROM_FIRST_RUN_ADDRESS, EEPROM_FIRST_RUN_MARK);
        EEPROM.commit();

        for (uint8_t i = 0; i < MODE_AMOUNT; i++)
        {
          EEPROM.put(EEPROM_MODES_START_ADDRESS + EEPROM_MODE_STRUCT_SIZE * i, modes[i]);
          EEPROM.commit();
        }
      }
      else
      // инициализируем настройки лампы значениями из EEPROM
      for (uint8_t i = 0; i < MODE_AMOUNT; i++)
      {
        EEPROM.get(EEPROM_MODES_START_ADDRESS + EEPROM_MODE_STRUCT_SIZE * i, modes[i]);
      }
      if (EEPROM.read(EEPROM_FIRST_RUN_ADDRESS + 2) != EEPROM_FIRST_RUN_MARK)
      {
             esp_task_wdt_reset();
        }     
      }

    static void EepromGet(ModeType modes[]) {
      for (uint8_t i = 0; i < MODE_AMOUNT; i++)
      {
        EEPROM.get(EEPROM_MODES_START_ADDRESS + EEPROM_MODE_STRUCT_SIZE * i, modes[i]);
      }
    }

    static void EepromPut(ModeType modes[]) {
      for (uint8_t i = 0; i < MODE_AMOUNT; i++)
      {
          EEPROM.put(EEPROM_MODES_START_ADDRESS + EEPROM_MODE_STRUCT_SIZE * i, modes[i]);
          EEPROM.commit();
      }
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
