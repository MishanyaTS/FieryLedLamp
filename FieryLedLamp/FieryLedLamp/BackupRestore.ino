// Резервное копирование / восстановление файлов настроек лампы

static const char* CFG_BACKUP_ZIP_PATH  = "/config_backup.zip";
static const char* CFG_RESTORE_ZIP_PATH = "/config_restore.zip";
static bool cfgRestoreSuccess = false;
static String cfgRestoreMessage;

struct BackupConfigFileInfo {
  const char* fsPath;
  const char* zipName;
  const __FlashStringHelper* displayName;
  size_t maxLen;
  bool required;
};

static const BackupConfigFileInfo BACKUP_CFG_FILES[] = {
  { "/config.json",            "config.json",            F("config.json"),            4096, false },
  { "/config_ir.json",         "config_ir.json",         F("config_ir.json"),         4096, false },
  { "/config_alarm.json",      "config_alarm.json",      F("config_alarm.json"),      2048, false },
  { "/config_cycle.json",      "config_cycle.json",      F("config_cycle.json"),      4096, false },
  { "/config_hardware.json",   "config_hardware.json",   F("config_hardware.json"),   2048, false },
  { "/config_ip.json",         "config_ip.json",         F("config_ip.json"),         2048, false },
  { "/config_mqtt.json",       "config_mqtt.json",       F("config_mqtt.json"),       2048, false },
  { "/config_multilamp.json",  "config_multilamp.json",  F("config_multilamp.json"),  2048, false },
  { "/config_sound.json",      "config_sound.json",      F("config_sound.json"),      4096, false },
  { "/config_sunset.json",     "config_sunset.json",     F("config_sunset.json"),     2048, false },
};

static const size_t BACKUP_CFG_FILE_COUNT = sizeof(BACKUP_CFG_FILES) / sizeof(BACKUP_CFG_FILES[0]);

static int backupFindConfigIndexByZipName(const String &name) {
  for (size_t i = 0; i < BACKUP_CFG_FILE_COUNT; i++) {
    String target(BACKUP_CFG_FILES[i].zipName);
    if (name == target || name == String('/') + target) return (int)i;
    int slash = name.lastIndexOf('/');
    if (slash >= 0 && name.substring(slash + 1) == target) return (int)i;
    int bslash = name.lastIndexOf('\\');
    if (bslash >= 0 && name.substring(bslash + 1) == target) return (int)i;
  }
  return -1;
}

static String backupMakeTmpRestorePath(size_t index) {
  String s = F("/restore_tmp_");
  s += index;
  s += F(".json");
  return s;
}

static inline void zipWriteU16(File &f, uint16_t v) {
  uint8_t b[2] = { (uint8_t)(v & 0xFF), (uint8_t)((v >> 8) & 0xFF) };
  f.write(b, 2);
}

static inline void zipWriteU32(File &f, uint32_t v) {
  uint8_t b[4] = {
    (uint8_t)(v & 0xFF),
    (uint8_t)((v >> 8) & 0xFF),
    (uint8_t)((v >> 16) & 0xFF),
    (uint8_t)((v >> 24) & 0xFF)
  };
  f.write(b, 4);
}

static bool zipReadU16(File &f, uint16_t &v) {
  uint8_t b[2];
  if (f.read(b, 2) != 2) return false;
  v = (uint16_t)b[0] | ((uint16_t)b[1] << 8);
  return true;
}

static bool zipReadU32(File &f, uint32_t &v) {
  uint8_t b[4];
  if (f.read(b, 4) != 4) return false;
  v = (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
  return true;
}

static uint32_t zipCrc32Update(uint32_t crc, const uint8_t* data, size_t len) {
  crc = ~crc;
  while (len--) {
    crc ^= *data++;
    for (uint8_t k = 0; k < 8; k++) {
      crc = (crc & 1U) ? (crc >> 1) ^ 0xEDB88320UL : (crc >> 1);
    }
  }
  return ~crc;
}

static bool zipCalcFileInfo(const char* path, uint32_t &sizeOut, uint32_t &crcOut) {
  File f = LittleFS.open(path, "r");
  if (!f) return false;

  sizeOut = 0;
  crcOut = 0;
  uint8_t buf[256];
  while (f.available()) {
    size_t n = f.read(buf, sizeof(buf));
    if (!n) break;
    sizeOut += n;
    crcOut = zipCrc32Update(crcOut, buf, n);
  }
  f.close();
  return true;
}

static bool zipCopyFileToOut(const char* path, File &out) {
  File in = LittleFS.open(path, "r");
  if (!in) return false;

  uint8_t buf[256];
  while (in.available()) {
    size_t n = in.read(buf, sizeof(buf));
    if (!n) break;
    if (out.write(buf, n) != n) {
      in.close();
      return false;
    }
  }
  in.close();
  return true;
}

static bool zipNameIsTarget(const String &name, const __FlashStringHelper* targetName) {
  String target(targetName);
  if (name == target || name == String('/') + target) return true;
  int slash = name.lastIndexOf('/');
  if (slash >= 0 && name.substring(slash + 1) == target) return true;
  int bslash = name.lastIndexOf('\\');
  if (bslash >= 0 && name.substring(bslash + 1) == target) return true;
  return false;
}


static bool zipReplaceFileWithValidatedTemp(const char* tmpPath, const char* dstPath) {
  String bakPath = String(dstPath) + F(".bak");
  LittleFS.remove(bakPath);
  bool hadOriginal = LittleFS.exists(dstPath);
  if (hadOriginal && !LittleFS.rename(dstPath, bakPath)) {
    cfgRestoreMessage = String(F("Не удалось подготовить замену ")) + dstPath;
    return false;
  }
  LittleFS.remove(dstPath);
  if (!LittleFS.rename(tmpPath, dstPath)) {
    if (hadOriginal) {
      LittleFS.rename(bakPath, dstPath);
    }
    cfgRestoreMessage = String(F("Не удалось применить ")) + dstPath;
    return false;
  }
  if (hadOriginal) LittleFS.remove(bakPath);
  return true;
}

static bool createConfigBackupZip() {
  struct ZipEntryInfo {
    const char* fsPath;
    const char* zipName;
    uint32_t size;
    uint32_t crc;
    uint32_t localOffset;
  };

  ZipEntryInfo entries[BACKUP_CFG_FILE_COUNT];
  size_t entryCount = 0;

  for (size_t i = 0; i < BACKUP_CFG_FILE_COUNT; i++) {
    if (!LittleFS.exists(BACKUP_CFG_FILES[i].fsPath)) {
      if (BACKUP_CFG_FILES[i].required) {
        cfgRestoreMessage = String(F("Не найден файл ")) + BACKUP_CFG_FILES[i].fsPath;
        return false;
      }
      continue;
    }
    entries[entryCount].fsPath = BACKUP_CFG_FILES[i].fsPath;
    entries[entryCount].zipName = BACKUP_CFG_FILES[i].zipName;
    entries[entryCount].size = 0;
    entries[entryCount].crc = 0;
    entries[entryCount].localOffset = 0;
    if (!zipCalcFileInfo(entries[entryCount].fsPath, entries[entryCount].size, entries[entryCount].crc)) {
      cfgRestoreMessage = String(F("Не найден файл ")) + entries[entryCount].fsPath;
      return false;
    }
    entryCount++;
  }

  if (entryCount == 0) {
    cfgRestoreMessage = F("Не найдено файлов настроек для архивации");
    return false;
  }

  LittleFS.remove(CFG_BACKUP_ZIP_PATH);
  File out = LittleFS.open(CFG_BACKUP_ZIP_PATH, "w");
  if (!out) {
    cfgRestoreMessage = F("Не удалось создать архив");
    return false;
  }

  for (size_t i = 0; i < entryCount; i++) {
    entries[i].localOffset = out.position();
    zipWriteU32(out, 0x04034B50UL);
    zipWriteU16(out, 20);
    zipWriteU16(out, 0);
    zipWriteU16(out, 0);
    zipWriteU16(out, 0);
    zipWriteU16(out, 0);
    zipWriteU32(out, entries[i].crc);
    zipWriteU32(out, entries[i].size);
    zipWriteU32(out, entries[i].size);
    zipWriteU16(out, strlen(entries[i].zipName));
    zipWriteU16(out, 0);
    out.write((const uint8_t*)entries[i].zipName, strlen(entries[i].zipName));

    if (!zipCopyFileToOut(entries[i].fsPath, out)) {
      out.close();
      cfgRestoreMessage = String(F("Не удалось добавить в архив ")) + entries[i].fsPath;
      return false;
    }
  }

  uint32_t centralDirOffset = out.position();
  for (size_t i = 0; i < entryCount; i++) {
    zipWriteU32(out, 0x02014B50UL);
    zipWriteU16(out, 20);
    zipWriteU16(out, 20);
    zipWriteU16(out, 0);
    zipWriteU16(out, 0);
    zipWriteU16(out, 0);
    zipWriteU16(out, 0);
    zipWriteU32(out, entries[i].crc);
    zipWriteU32(out, entries[i].size);
    zipWriteU32(out, entries[i].size);
    zipWriteU16(out, strlen(entries[i].zipName));
    zipWriteU16(out, 0);
    zipWriteU16(out, 0);
    zipWriteU16(out, 0);
    zipWriteU16(out, 0);
    zipWriteU32(out, 0);
    zipWriteU32(out, entries[i].localOffset);
    out.write((const uint8_t*)entries[i].zipName, strlen(entries[i].zipName));
  }

  uint32_t centralDirSize = out.position() - centralDirOffset;
  zipWriteU32(out, 0x06054B50UL);
  zipWriteU16(out, 0);
  zipWriteU16(out, 0);
  zipWriteU16(out, entryCount);
  zipWriteU16(out, entryCount);
  zipWriteU32(out, centralDirSize);
  zipWriteU32(out, centralDirOffset);
  zipWriteU16(out, 0);
  out.close();
  return true;
}

static void sendBackupRestorePage(const String &msg = String(), bool ok = false) {
  String html;
  html.reserve(3200);
  html += F("<!doctype html><html><head><meta charset='utf-8'>");
  html += F("<meta name='viewport' content='width=device-width,initial-scale=1'>");
  html += F("<title>Резервная копия настроек</title>");
  html += F("<style>");
  html += F("body{font-family:Arial,sans-serif;background:#10151d;color:#eef;max-width:680px;margin:0 auto;padding:16px;box-sizing:border-box}");
  html += F("*,*:before,*:after{box-sizing:border-box}");
  html += F(".card{background:#182433;border-radius:14px;padding:16px;margin:14px 0;box-shadow:0 4px 18px rgba(0,0,0,.25);overflow:hidden}");
  html += F(".btn{display:block;width:100%;max-width:100%;padding:12px 14px;margin:10px 0;border:0;border-radius:10px;text-decoration:none;text-align:center;font-size:14px;line-height:1.35;white-space:normal;word-break:break-word;overflow-wrap:anywhere;color:#fff;background:#1f7ae0}");
  html += F(".btn2{background:#2f9d59}");
  html += F("input[type=file]{display:block;width:100%;max-width:100%;padding:10px;background:#0f1722;color:#eef;border:1px solid #38506e;border-radius:10px;line-height:1.35}");
  html += F(".msg{padding:12px;border-radius:10px;margin:10px 0;background:");
  if (ok) html += F("#1f5f35;color:#d7ffd7}");
  else    html += F("#693232;color:#ffdede}");
  html += F("small{color:#aac3df}h2,h3{margin:0 0 12px} .back{background:#48566a}");
  html += F("</style></head><body>");
  html += F("<div class='card'><h2>Резервная копия настроек лампы</h2>");
  html += F("<small>Архив содержит config.json, config_ir.json, config_alarm.json, config_cycle.json, config_hardware.json, config_ip.json, config_mqtt.json, config_multilamp.json, config_sound.json и config_sunset.json.</small>");
  if (msg.length()) {
    html += F("<div class='msg'>");
    html += msg;
    html += F("</div>");
  }
  html += F("<a class='btn' href='/backup_config'>Сохранить настройки лампы</a>");
  html += F("</div>");
  html += F("<div class='card'><h3>Загрузить настройки лампы</h3>");
  html += F("<form method='post' action='/restore_config' enctype='multipart/form-data'>");
  html += F("<label for='archive'><small>Выберите ZIP-архив с настройками</small></label>");
  html += F("<input id='archive' type='file' name='archive' accept='.zip' required>");
  html += F("<button class='btn btn2' type='submit'>Загрузить архив настроек</button>");
  html += F("</form><small>Поддерживается архив, созданный кнопкой сохранения на этой лампе. Можно восстанавливать и неполный архив: будут применены только найденные файлы.</small></div>");
  html += F("<a class='btn back' href='/?user_seting'>Назад</a>");
  html += F("</body></html>");
  HTTP.send(200, F("text/html"), html);
}

static bool restoreConfigFromZip(const char* zipPath) {
  File in = LittleFS.open(zipPath, "r");
  if (!in) {
    cfgRestoreMessage = F("Архив не открыт");
    return false;
  }

  bool gotFiles[BACKUP_CFG_FILE_COUNT];
  String tmpPaths[BACKUP_CFG_FILE_COUNT];
  for (size_t i = 0; i < BACKUP_CFG_FILE_COUNT; i++) {
    gotFiles[i] = false;
    tmpPaths[i] = backupMakeTmpRestorePath(i);
    LittleFS.remove(tmpPaths[i]);
  }

  while (in.available()) {
    uint32_t sig = 0;
    if (!zipReadU32(in, sig)) break;
    if (sig == 0x02014B50UL || sig == 0x06054B50UL) {
      break;
    }
    if (sig != 0x04034B50UL) {
      cfgRestoreMessage = F("Неверный формат ZIP");
      in.close();
      return false;
    }

    uint16_t verNeeded, flags, method, modTime, modDate, nameLen, extraLen;
    uint32_t crc, compSize, uncompSize;
    if (!zipReadU16(in, verNeeded) || !zipReadU16(in, flags) || !zipReadU16(in, method) ||
        !zipReadU16(in, modTime) || !zipReadU16(in, modDate) || !zipReadU32(in, crc) ||
        !zipReadU32(in, compSize) || !zipReadU32(in, uncompSize) ||
        !zipReadU16(in, nameLen) || !zipReadU16(in, extraLen)) {
      cfgRestoreMessage = F("Повреждён заголовок ZIP");
      in.close();
      return false;
    }

    if ((flags & 0x0008U) != 0U) {
      cfgRestoreMessage = F("ZIP с data descriptor пока не поддерживается");
      in.close();
      return false;
    }

    if (method != 0) {
      cfgRestoreMessage = F("Поддерживается только ZIP без сжатия (store)");
      in.close();
      return false;
    }

    String name;
    name.reserve(nameLen);
    for (uint16_t i = 0; i < nameLen; i++) {
      int c = in.read();
      if (c < 0) {
        cfgRestoreMessage = F("Ошибка чтения имени файла");
        in.close();
        return false;
      }
      name += (char)c;
    }

    if (extraLen) in.seek(in.position() + extraLen, SeekSet);

    int cfgIndex = backupFindConfigIndexByZipName(name);
    if (cfgIndex >= 0) {
      File out = LittleFS.open(tmpPaths[cfgIndex], "w");
      if (!out) {
        cfgRestoreMessage = String(F("Не удалось записать ")) + BACKUP_CFG_FILES[cfgIndex].fsPath;
        in.close();
        return false;
      }

      bool ok = true;
      {
        uint8_t buf[256];
        uint32_t left = compSize;
        while (left > 0) {
          size_t chunk = left > sizeof(buf) ? sizeof(buf) : left;
          size_t n = in.read(buf, chunk);
          if (n != chunk) {
            cfgRestoreMessage = F("Ошибка чтения архива");
            ok = false;
            break;
          }
          if (out.write(buf, n) != n) {
            cfgRestoreMessage = F("Ошибка записи файла настроек");
            ok = false;
            break;
          }
          left -= n;
        }
      }

      out.close();
      if (!ok) {
        LittleFS.remove(tmpPaths[cfgIndex]);
        in.close();
        return false;
      }

      gotFiles[cfgIndex] = true;
    } else {
      in.seek(in.position() + compSize, SeekSet);
    }
  }

  in.close();

  bool appliedAny = false;
  for (size_t i = 0; i < BACKUP_CFG_FILE_COUNT; i++) {
    if (!gotFiles[i]) continue;
    if (!zipReplaceFileWithValidatedTemp(tmpPaths[i].c_str(), BACKUP_CFG_FILES[i].fsPath)) {
      for (size_t j = 0; j < BACKUP_CFG_FILE_COUNT; j++) LittleFS.remove(tmpPaths[j]);
      return false;
    }
    appliedAny = true;
  }

  if (!appliedAny) {
    cfgRestoreMessage = F("В архиве не найдено поддерживаемых файлов настроек");
    return false;
  }

  configSetup = readFile(F("config.json"), 4096);
#if USE_IR_RECEIVER
  IR_LoadConfigFromFile();
#endif
  cfgRestoreMessage = F("Настройки загружены. Найденные файлы применены. Лампа будет перезагружена.");
  return true;
}

static void handleBackupConfigDownload() {
  if (!createConfigBackupZip()) {
    HTTP.send(500, F("text/plain"), cfgRestoreMessage.length() ? cfgRestoreMessage : F("Archive error"));
    return;
  }

  File file = LittleFS.open(CFG_BACKUP_ZIP_PATH, "r");
  if (!file) {
    HTTP.send(500, F("text/plain"), F("Archive open error"));
    return;
  }

  HTTP.sendHeader(F("Content-Type"), F("application/zip"));
  HTTP.sendHeader(F("Content-Disposition"), F("attachment; filename=lamp_settings.zip"));
  HTTP.sendHeader(F("Connection"), F("close"));
  HTTP.streamFile(file, F("application/zip"));
  file.close();
}

static void handleRestoreConfigUpload() {
  if (HTTP.uri() != "/restore_config") return;
  HTTPUpload& upload = HTTP.upload();

  if (upload.status == UPLOAD_FILE_START) {
    cfgRestoreSuccess = false;
    cfgRestoreMessage = "";
    LittleFS.remove(CFG_RESTORE_ZIP_PATH);
    fsUploadFile = LittleFS.open(CFG_RESTORE_ZIP_PATH, "w");
    if (!fsUploadFile) {
      cfgRestoreMessage = F("Не удалось создать временный файл");
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) fsUploadFile.close();
    cfgRestoreSuccess = restoreConfigFromZip(CFG_RESTORE_ZIP_PATH);
  }
}

static void handleRestoreConfigFinish() {
  if (cfgRestoreSuccess) {
    String html;
    html.reserve(900);
    html += F("<!doctype html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>");
    html += F("<title>Настройки загружены</title></head><body style='font-family:Arial,sans-serif;background:#10151d;color:#eef;text-align:center;padding:30px'>");
    html += F("<h3>Настройки загружены</h3><p>");
    html += cfgRestoreMessage;
    html += F("</p><p>Перезагрузка...</p><script>setTimeout(function(){fetch('/restart?device=ok').finally(function(){setTimeout(function(){location.href='/'},2500);});},800);</script></body></html>");
    HTTP.send(200, F("text/html"), html);
  } else {
    sendBackupRestorePage(cfgRestoreMessage.length() ? cfgRestoreMessage : String(F("Ошибка загрузки архива")), false);
  }
  LittleFS.remove(CFG_RESTORE_ZIP_PATH);
}

void BackupRestoreInit() {
  HTTP.on("/backup_restore", HTTP_GET, []() {
    sendBackupRestorePage();
  });

  HTTP.on("/backup_config", HTTP_GET, handleBackupConfigDownload);
  HTTP.on("/restore_config", HTTP_POST, handleRestoreConfigFinish, handleRestoreConfigUpload);
}
