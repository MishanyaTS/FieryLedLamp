#include <Update.h>
#include <WiFiClientSecure.h>

#ifndef OTA_PACKAGE_URL
#define OTA_PACKAGE_URL "https://github.com/MishanyaTS/FieryLedLamp/releases/latest/download/update.zip"
#endif

#ifndef OTA_GITHUB_API_LATEST
#define OTA_GITHUB_API_LATEST "https://api.github.com/repos/MishanyaTS/FieryLedLamp/releases/latest"
#endif

#ifndef OTA_PACKAGE_ASSET_NAME
#define OTA_PACKAGE_ASSET_NAME "update.zip"
#endif

static String otaPkgStatus = F("Готово");
static bool otaPkgBusy = false;
static String otaLatestVersion = "";
static String otaLatestDate = "";
static String otaLatestUrl = "";
static String otaLatestNotes = "";
static bool otaHasUpdate = false;
static uint32_t otaLastCheckMs = 0;
static String otaLastCheckMsg = "Не проверялось";

static bool otaPkgPending = false;
static bool otaPkgPendingSaveCfg = false;

bool saveConfigBackupToPartition(bool setPendingFlag);
void clearConfigRestorePending();
String getConfigRestoreMessage();
void otaSetPackageStatus(const String& status) {
  otaPkgStatus = status;
}

static void otaHandleHttpTick() {
  HTTP.handleClient();
  delay(1);
  yield();
}

static void otaStatusPause(uint32_t ms) {
  uint32_t started = millis();
  while (millis() - started < ms) {
    otaHandleHttpTick();
  }
}

static float otaVersionToFloat(String s) {
  s.trim();
  String out;
  bool dot = false;
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c >= '0' && c <= '9') out += c;
    else if (c == '.' && !dot) { out += c; dot = true; }
  }
  if (out.length() == 0) return 0.0f;
  return out.toFloat();
}

static bool otaParseUrl(const String& url, String& host, uint16_t& port, String& path) {
  host = "";
  path = "/";
  port = 0;
  String u = url;
  u.trim();
  if (u.startsWith("https://")) {
    port = 443;
    u.remove(0, 8);
  } else if (u.startsWith("http://")) {
    port = 80;
    u.remove(0, 7);
  } else {
    return false;
  }

  int slash = u.indexOf('/');
  String hostPort = (slash >= 0) ? u.substring(0, slash) : u;
  path = (slash >= 0) ? u.substring(slash) : "/";
  if (path.length() == 0) path = "/";

  int colon = hostPort.indexOf(':');
  if (colon >= 0) {
    host = hostPort.substring(0, colon);
    port = (uint16_t) hostPort.substring(colon + 1).toInt();
  } else {
    host = hostPort;
  }
  return host.length() > 0 && port > 0;
}

static bool otaReadHeaders(WiFiClientSecure& client, int& code, int& contentLength, String& location, String& message) {
  code = -1;
  contentLength = -1;
  location = "";

  String status = client.readStringUntil('\n');
  status.trim();
  if (!status.startsWith("HTTP/1.")) {
    message = F("Некорректный HTTP ответ");
    return false;
  }

  int sp1 = status.indexOf(' ');
  int sp2 = status.indexOf(' ', sp1 + 1);
  if (sp1 < 0) {
    message = F("Не удалось разобрать HTTP статус");
    return false;
  }
  code = status.substring(sp1 + 1, sp2 > 0 ? sp2 : status.length()).toInt();

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r" || line.length() == 0) break;
    line.trim();
    String low = line;
    low.toLowerCase();
    if (low.startsWith("content-length:")) {
      contentLength = line.substring(15).toInt();
    } else if (low.startsWith("location:")) {
      location = line.substring(9);
      location.trim();
    }
  }
  return true;
}

static bool otaHttpGetOpen(const String& url, WiFiClientSecure& client, int& code, int& contentLength, String& message, uint8_t redirects = 0) {
  if (redirects > 5) {
    message = F("Слишком много редиректов");
    return false;
  }

  String host, path, location;
  uint16_t port = 0;
  if (!otaParseUrl(url, host, port, path)) {
    message = F("Некорректный URL");
    return false;
  }

  client.stop();
  client.setInsecure();
  client.setTimeout(60000);

  if (!client.connect(host.c_str(), port)) {
    message = String(F("Не удалось подключиться к ")) + host;
    return false;
  }

  client.print(String(F("GET ")) + path + F(" HTTP/1.1\r\n") +
               F("Host: ") + host + F("\r\n") +
               F("User-Agent: FieryLedLamp-OTA\r\n") +
               F("Accept: */*\r\n") +
               F("Connection: close\r\n\r\n"));

  if (!otaReadHeaders(client, code, contentLength, location, message)) {
    client.stop();
    return false;
  }

  if (code == 301 || code == 302 || code == 303 || code == 307 || code == 308) {
    client.stop();
    if (location.length() == 0) {
      message = F("Редирект без Location");
      return false;
    }
    return otaHttpGetOpen(location, client, code, contentLength, message, redirects + 1);
  }

  return true;
}

static bool otaFetchTextUrl(const String& url, String& payload, String& message) {
  WiFiClientSecure client;
  int code = -1;
  int contentLength = -1;
  if (!otaHttpGetOpen(url, client, code, contentLength, message)) return false;

  if (code != 200) {
    message = String(F("HTTP ошибка: ")) + code;
    client.stop();
    return false;
  }

  payload = "";
  uint32_t started = millis();
  while (client.connected() || client.available()) {
    while (client.available()) {
      payload += client.readStringUntil('\n') + "\n";
      started = millis();
      yield();
    }
    if (millis() - started > 60000UL) break;
    delay(1);
    yield();
  }
  client.stop();
  return payload.length() > 0;
}

static bool otaFetchGitHubLatest(String& latestVer, String& latestDate, String& assetUrl, String& releaseNotes, String& message) {
  latestVer = "";
  latestDate = "";
  assetUrl = "";
  releaseNotes = "";

  if (WiFi.status() != WL_CONNECTED) {
    message = F("WiFi не подключен");
    return false;
  }

  String payload;
  if (!otaFetchTextUrl(String(F(OTA_GITHUB_API_LATEST)), payload, message)) {
    if (message.length() == 0) message = F("Пустой ответ GitHub API");
    return false;
  }

  DynamicJsonDocument doc(12288);
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    message = String(F("Ошибка JSON GitHub: ")) + err.c_str();
    return false;
  }

  latestVer = String((const char*)(doc["tag_name"] | doc["name"] | ""));
  latestDate = String((const char*)(doc["published_at"] | doc["created_at"] | ""));
  releaseNotes = String((const char*)(doc["body"] | ""));
  releaseNotes.replace("\r\n", "\n");
  releaseNotes.replace("\r", "\n");
  releaseNotes.trim();

  JsonArray assets = doc["assets"].as<JsonArray>();
  for (JsonObject asset : assets) {
    String n = String((const char*)(asset["name"] | ""));
    if (n.equalsIgnoreCase(F(OTA_PACKAGE_ASSET_NAME))) {
      assetUrl = String((const char*)(asset["browser_download_url"] | ""));
      break;
    }
  }

  if (latestVer.length() == 0) {
    message = F("GitHub не вернул tag_name");
    return false;
  }
  if (assetUrl.length() == 0) {
    message = String(F("В последнем выпуске нет актива ")) + F(OTA_PACKAGE_ASSET_NAME);
    return false;
  }

  message = F("OK");
  return true;
}

static void otaStoreCheckResult(const String& latestVer, const String& latestDate, const String& latestUrl, const String& latestNotes, bool hasUpdate, const String& message) {
  otaLatestVersion = latestVer;
  otaLatestDate = latestDate;
  otaLatestUrl = latestUrl;
  otaLatestNotes = latestNotes;
  otaHasUpdate = hasUpdate;
  otaLastCheckMsg = message;
  otaLastCheckMs = millis();
}

static uint16_t ota_le16(const uint8_t* p) {
  return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}
static uint32_t ota_le32(const uint8_t* p) {
  return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

class OtaZipStreamUpdater {
  public:
    enum FileType : uint8_t { FT_NONE = 0, FT_FW, FT_FS };
    enum State : uint8_t { ST_HEADER = 0, ST_NAME, ST_EXTRA, ST_DATA, ST_DONE, ST_FAIL };

    bool begin() {
      reset();
      return true;
    }

    bool feed(uint8_t* data, size_t len) {
      size_t pos = 0;
      while (pos < len) {
        switch (state) {
          case ST_HEADER: {
            size_t need = 30 - hdrUsed;
            size_t take = (len - pos < need) ? (len - pos) : need;
            memcpy(header + hdrUsed, data + pos, take);
            hdrUsed += take;
            pos += take;
            if (hdrUsed == 30) {
              if (ota_le32(header) != 0x04034B50UL) {
                state = ST_DONE;
                break;
              }
              uint16_t flags = ota_le16(header + 6);
              method = ota_le16(header + 8);
              compSize = ota_le32(header + 18);
              uncompSize = ota_le32(header + 22);
              nameLen = ota_le16(header + 26);
              extraLen = ota_le16(header + 28);

              if (flags & 0x0008) {
                fail(F("ZIP с data descriptor не поддерживается"));
                return false;
              }
              if (method != 0) {
                fail(F("ZIP должен быть без сжатия (store)"));
                return false;
              }
              if (nameLen >= sizeof(nameBuf)) {
                fail(F("Слишком длинное имя файла в ZIP"));
                return false;
              }
              nameUsed = 0;
              state = ST_NAME;
            }
          } break;

          case ST_NAME: {
            size_t need = nameLen - nameUsed;
            size_t take = (len - pos < need) ? (len - pos) : need;
            memcpy(nameBuf + nameUsed, data + pos, take);
            nameUsed += take;
            pos += take;
            if (nameUsed == nameLen) {
              nameBuf[nameLen] = '\0';
              String n = String(nameBuf);
              int slash = n.lastIndexOf('/');
              if (slash >= 0) n = n.substring(slash + 1);
              currentName = n;
              extraLeft = extraLen;
              state = ST_EXTRA;
            }
          } break;

          case ST_EXTRA: {
            size_t take = (len - pos < extraLeft) ? (len - pos) : extraLeft;
            pos += take;
            extraLeft -= take;
            if (extraLeft == 0) {
              fileLeft = compSize;
              if (!startEntryUpdate()) return false;
              state = ST_DATA;
            }
          } break;

          case ST_DATA: {
            size_t chunk = (len - pos < fileLeft) ? (len - pos) : fileLeft;
            if (chunk > 0) {
              if (currentType != FT_NONE) {
                uint8_t* wr = (uint8_t*)(data + pos);
                size_t written = Update.write(wr, chunk);
                if (written != chunk) {
                  fail(String(F("Ошибка записи OTA: ")) + Update.errorString());
                  return false;
                }
              }
              pos += chunk;
              fileLeft -= chunk;
            }
            if (fileLeft == 0) {
              if (currentType != FT_NONE) {
                if (!endEntryUpdate()) return false;
              }
              hdrUsed = 0;
              state = ST_HEADER;
            }
          } break;

          case ST_DONE:
            pos = len;
            break;

          case ST_FAIL:
            return false;
        }
      }
      return true;
    }

    bool finish() {
      if (state == ST_FAIL) return false;
      if (updateOpen) {
        if (!endEntryUpdate()) return false;
      }
      if (!seenFw && !seenFs) {
        fail(F("В ZIP нет firmware.bin или littlefs.bin"));
        return false;
      }
      return true;
    }

    const String& error() const { return err; }
    bool hasFirmware() const { return seenFw; }
    bool hasFs() const { return seenFs; }

  private:
    State state = ST_HEADER;
    FileType currentType = FT_NONE;
    uint8_t header[30];
    size_t hdrUsed = 0;
    uint16_t method = 0;
    uint32_t compSize = 0;
    uint32_t uncompSize = 0;
    uint16_t nameLen = 0;
    uint16_t extraLen = 0;
    uint16_t extraLeft = 0;
    uint32_t fileLeft = 0;
    char nameBuf[96];
    uint16_t nameUsed = 0;
    String currentName;
    String err;
    bool updateOpen = false;
    bool seenFw = false;
    bool seenFs = false;

    void reset() {
      state = ST_HEADER;
      currentType = FT_NONE;
      hdrUsed = 0;
      method = 0;
      compSize = 0;
      uncompSize = 0;
      nameLen = 0;
      extraLen = 0;
      extraLeft = 0;
      fileLeft = 0;
      nameUsed = 0;
      currentName = "";
      err = "";
      updateOpen = false;
      seenFw = false;
      seenFs = false;
    }

    void fail(const String& e) {
      err = e;
      if (updateOpen) Update.abort();
      updateOpen = false;
      state = ST_FAIL;
    }

    bool startEntryUpdate() {
      currentType = FT_NONE;
      updateOpen = false;

      if (currentName.equalsIgnoreCase(F("firmware.bin")) || currentName.equalsIgnoreCase(F("FieryLedLamp.ino.bin"))) {
        currentType = FT_FW;
      } else if (currentName.equalsIgnoreCase(F("littlefs.bin")) || currentName.equalsIgnoreCase(F("FieryLedLamp.littlefs.bin"))) {
        currentType = FT_FS;
      } else {
        return true;
      }

      if (uncompSize == 0 || compSize == 0) {
        fail(String(F("Пустой файл в ZIP: ")) + currentName);
        return false;
      }

      int cmd = (currentType == FT_FW) ? U_FLASH : U_SPIFFS;
      if (!Update.begin(uncompSize, cmd)) {
        fail(String(F("Не удалось начать OTA для ")) + currentName + F(": ") + Update.errorString());
        return false;
      }
      updateOpen = true;
      return true;
    }

    bool endEntryUpdate() {
      if (!Update.end(true)) {
        fail(String(F("Ошибка завершения OTA для ")) + currentName + F(": ") + Update.errorString());
        return false;
      }
      if (currentType == FT_FW) seenFw = true;
      if (currentType == FT_FS) seenFs = true;
      updateOpen = false;
      currentType = FT_NONE;
      return true;
    }
};

static bool otaUpdateFromStream(Stream& stream, int contentLength, String& message) {
  OtaZipStreamUpdater updater;
  updater.begin();

  uint8_t buf[2048];
  int remaining = contentLength;
  uint32_t lastDataMs = millis();
  uint32_t lastHttpTickMs = 0;

  while (true) {
    size_t avail = stream.available();
    if (avail > sizeof(buf)) avail = sizeof(buf);

    if (avail > 0) {
      int rd = stream.readBytes((char*)buf, avail);
      if (rd > 0) {
        if (!updater.feed(buf, (size_t)rd)) {
          message = updater.error();
          return false;
        }
        lastDataMs = millis();
        if (remaining > 0) {
          remaining -= rd;
          if (remaining <= 0) break;
        }
      }
    } else {
      if (contentLength > 0 && remaining <= 0) break;
      if (millis() - lastDataMs > 60000UL) {
        message = F("Таймаут чтения OTA ZIP");
        return false;
      }
      otaHandleHttpTick();
    }

    if (millis() - lastHttpTickMs >= 250UL) {
      otaHandleHttpTick();
      lastHttpTickMs = millis();
    }

    if (contentLength < 0 && !stream.available()) {
      if (millis() - lastDataMs > 2000UL) break;
    }
  }

  if (!updater.finish()) {
    message = updater.error();
    return false;
  }

  message = F("OK");
  if (updater.hasFirmware() && updater.hasFs()) message = F("Прошивка + ФС");
  else if (updater.hasFirmware()) message = F("Прошивка");
  else if (updater.hasFs()) message = F("ФС");
  return true;
}

static bool otaUpdateFromGitHubUrl(const String& url, String& message) {
  if (WiFi.status() != WL_CONNECTED) {
    message = F("WiFi не подключен");
    return false;
  }

  WiFiClientSecure client;
  int code = -1;
  int len = -1;
  if (!otaHttpGetOpen(url, client, code, len, message)) return false;

  if (code != 200) {
    message = String(F("HTTP ошибка: ")) + code;
    client.stop();
    return false;
  }

  bool ok = otaUpdateFromStream(client, len, message);
  client.stop();
  return ok;
}

static String otaBuildAppliedStatus(const String& result) {
  String s = result;
  s.trim();
  if (!s.length()) return F("Обновление завершено");
  return String(F("Обновление завершено: ")) + s;
}

void OtaPackageHandle() {
  if (!otaPkgPending) return;

  otaPkgPending = false;
  otaPkgBusy = true;

  if (!saveEffectSettingsNow(false)) {
    otaPkgBusy = false;
    otaPkgStatus = F("Ошибка: не удалось сохранить настройки эффектов в EEPROM");
    return;
  }

  if (otaPkgPendingSaveCfg) {
    // Сохраняем config*.json и старый /effect.ini. После установки
    // littlefs.bin старая и новая версии effect.ini будут проверены и слиты.
    if (!saveConfigBackupToPartition(true)) {
      otaPkgBusy = false;
      String err = getConfigRestoreMessage();
      otaPkgStatus = String(F("Ошибка: ")) + (err.length() ? err : String(F("Не удалось сохранить настройки")));
      return;
    }

    otaPkgStatus = F("Настройки и effect.ini сохранены");
    otaStatusPause(800);
  } else {
    clearConfigRestorePending();
  }

  EepromManager::SaveWifiBackupForGitHubOta(configSetup);
  otaPkgStatus = F("Обновление...");
  otaStatusPause(250);

  String msg;
  String url = otaLatestUrl.length() ? otaLatestUrl : String(F(OTA_PACKAGE_URL));
  bool ok = otaUpdateFromGitHubUrl(url, msg);

  if (!ok) {
    EepromManager::ClearWifiBackupPending();
    if (otaPkgPendingSaveCfg) clearConfigRestorePending();
    otaPkgStatus = String(F("Ошибка: ")) + msg;
    otaPkgBusy = false;
    return;
  }

  otaPkgStatus = otaBuildAppliedStatus(msg);
  otaStatusPause(1200);

  otaPkgStatus = F("Перезагрузка");
  otaStatusPause(250);
  ESP.restart();
}

void OtaPackageInit() {
  HTTP.on("/ota_package_status", HTTP_GET, []() {
    HTTP.send(200, "text/plain; charset=utf-8", otaPkgStatus);
  });

  HTTP.on("/ota_package_check", HTTP_GET, []() {
    DynamicJsonDocument doc(2048);
    String latestVer, latestDate, assetUrl, latestNotes, msg;
    bool ok = otaFetchGitHubLatest(latestVer, latestDate, assetUrl, latestNotes, msg);
    String currentVer = String(F(VERSION));
    currentVer.trim();

    bool hasUpdate = false;
    if (ok) {
      hasUpdate = otaVersionToFloat(latestVer) > otaVersionToFloat(currentVer);
      otaStoreCheckResult(latestVer, latestDate, assetUrl, latestNotes, hasUpdate, msg);
    } else {
      otaStoreCheckResult("", "", "", "", false, msg);
    }

    doc["ok"] = ok;
    doc["busy"] = otaPkgBusy;
    doc["current_version"] = currentVer;
    doc["latest_version"] = latestVer;
    doc["latest_date"] = latestDate;
    doc["asset_url"] = assetUrl;
    doc["update_available"] = hasUpdate;
    doc["message"] = msg;

    String out;
    serializeJson(doc, out);
    HTTP.send(ok ? 200 : 500, "application/json; charset=utf-8", out);
  });

  HTTP.on("/ota_package_notes", HTTP_GET, []() {
    if (otaLatestNotes.length() > 0) {
      HTTP.send(200, "text/plain; charset=utf-8", otaLatestNotes);
      return;
    }

    String latestVer, latestDate, assetUrl, latestNotes, msg;
    bool ok = otaFetchGitHubLatest(latestVer, latestDate, assetUrl, latestNotes, msg);
    if (!ok) {
      HTTP.send(500, "text/plain; charset=utf-8", msg.length() ? msg : String(F("Не удалось получить описание изменений")));
      return;
    }

    String currentVer = String(F(VERSION));
    currentVer.trim();
    bool hasUpdate = otaVersionToFloat(latestVer) > otaVersionToFloat(currentVer);
    otaStoreCheckResult(latestVer, latestDate, assetUrl, latestNotes, hasUpdate, msg);

    if (latestNotes.length() == 0) latestNotes = F("Описание изменений не указано.");
    HTTP.send(200, "text/plain; charset=utf-8", latestNotes);
  });

  HTTP.on("/update_package_github_fixed", HTTP_GET, []() {
    if (otaPkgBusy || otaPkgPending) {
      HTTP.send(409, "text/plain; charset=utf-8", "OTA уже выполняется");
      return;
    }

    otaPkgPendingSaveCfg = HTTP.hasArg("savecfg") && HTTP.arg("savecfg") == "1";
    otaPkgPending = true;
    otaPkgStatus = F("Подготовка...");

    HTTP.send(200, "text/plain; charset=utf-8", otaPkgStatus);
  });
}
