//
// ======================= ВНИМАНИЕ !!! =============================
// Все настройки производятся на вкладке Constants.h
// Почитайте там то, что на русском языке написано.
// Или ничего не трогайте, если собирали, по схемам из этого архива.
// В любом случае ВНИМАТЕЛЬНО прочтите файл ПРОЧТИ МЕНЯ!!!.txt из этого архива.
// ==================================================================
// Ссылка для менеджера плат:
// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
//
// ESP32-S3:
// При установке выбираем версию 2.0.14
// Выбираем плату ESP32S3 Dev Module
// USB CDC On Boot: "Enabled"
// Flash Size: "16MB (128MB)"
// PSRAM: "OPI PSRAM"
// Размер памяти "16MB with spiffs (6.25MB_backup APP/3.43MB SPIFFS)"
// =================================================================
#if !defined(CONFIG_IDF_TARGET_ESP32S3)
  #error "Этот проект поддерживает только ESP32-S3. Выберите плату ESP32S3 Dev Module."
#endif
#define ESP32_USED
#define ESP32_S3_USED
#define CORE_2_0_X
//===================================================================================================
// Далее следует код проекта. Не меняйте здесь ничего, если вы не понимаете, к чему это приведет!!!
//===================================================================================================

#include <pgmspace.h>
#include "esp_wifi.h"                // Борьба с рестартом esp32 "assertion "Invalid mbox""
#include "nvs_flash.h"               // Борьба с рестартом esp32 "assertion "Invalid mbox""
#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#include <WiFiAP.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <ESP32SSDP.h>               // https://github.com/luc-github/ESP32SSDP
#include <time.h>
#include <HardwareSerial.h>          // Используется аппаратный UART
#include "esp_system.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include <ElegantOTA.h>

#include <FastLED.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <TimeLib.h>
#include "SystemLog.h"
#include "Constants.h"

#if USE_RTC
  #ifdef RTC_3231
  #include <Wire.h>
    #include <RtcDS3231.h>
  #endif
#endif
#if USE_BUTTON
#include <GyverButton.h>
#endif
#ifdef USE_NTP
#include <NTPClient.h>
#include <Timezone.h>
#endif
#include "Types.h"
#include "timerMinim.h"
#include "fonts.h"
#if USE_OTA
#include "OtaManager.h"
#endif
#if USE_MQTT
#include "MqttManager.h"
#endif
#include "EepromManager.h"
#include "FavoritesManager.h"

bool restoreConfigBackupFromPartition(bool useEepromEffectFallback);
bool mergeEffectSettingsForRestore(const char* savedFileName,
                                   const char* newFileName,
                                   bool useEepromFallback);
bool isConfigRestorePending();
void clearConfigRestorePending();
#include "TimerManager.h"
#if USE_BLYNK
String blynkToken;
bool blynkEnabled = false;
bool blynkConfigured = false;
String blynkConfiguredToken;
bool blynkConfiguredEnabled = false;
#include <BlynkSimpleEsp32.h>
#endif
#if USE_TM1637
#include "TM1637Display.h"
#endif
#if USE_MP3_PLAYER
 #ifdef MP3_DEBUG
  #define FEEDBACK  1
 #else
  #define FEEDBACK  0
 #endif  //MP3_DEBUG
#endif  // USE_MP3_PLAYER
#if USE_IR_RECEIVER
 #include <IRremoteESP8266.h>  // https://github.com/crankyoldgit/IRremoteESP8266
 #include <IRrecv.h>
 #include "IR_Receiver.h"
#endif  //USE_IR_RECEIVER

// --- ИНИЦИАЛИЗАЦИЯ ОБЪЕКТОВ ----------
#if USE_RTC
  #ifdef RTC_3231
    RtcDS3231<TwoWire> Rtc(Wire);
  #endif
RtcDateTime timeToSet;
#endif

uint8_t matrixWidth = WIDTH_DEFAULT;
uint8_t matrixHeight = HEIGHT_DEFAULT;
uint8_t segWidth = WIDTH_DEFAULT;                 // Размер одной физической матрицы по ширине
uint8_t segHeight = HEIGHT_DEFAULT;                // Размер одной физической матрицы по высоте
uint8_t segMatrixW = 1U;                           // Количество одинаковых матриц по горизонтали
uint8_t segMatrixH = 1U;                           // Количество одинаковых матриц по вертикали
bool panelFlip = false;                            // Переворот всей панели на 180°
uint8_t ledDataLines = 2U;
CRGB leds[NUM_LEDS_MAX];
WiFiUDP Udp;
bool apFallbackActive = false;
uint32_t apFallbackStartMs = 0;
uint32_t lastRouterRetryMs = 0;

#ifdef USE_NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, 0, NTP_INTERVAL); // объект, запрашивающий время с ntp сервера; в нём смещение часового пояса не используется (перенесено в объект localTimeZone); здесь всегда должно быть время UTC
   TimeChangeRule summerTime  = { SUMMER_TIMEZONE_NAME, SUMMER_WEEK_NUM, SUMMER_WEEKDAY, SUMMER_MONTH, SUMMER_HOUR, 0 };
   TimeChangeRule winterTime  = { WINTER_TIMEZONE_NAME, WINTER_WEEK_NUM, WINTER_WEEKDAY, WINTER_MONTH, WINTER_HOUR, 0 };
   Timezone localTimeZone(summerTime, winterTime);
  #ifdef PHONE_N_MANUAL_TIME_PRIORITY
    bool stillUseNTP = true;
  #endif    
#endif

timerMinim timeTimer(3000);
bool ntpServerAddressResolved = false;
bool timeSynched = false;
uint32_t lastTimePrinted = 0U;
uint32_t lastWeatherPrinted = 0U;

#if defined(USE_MANUAL_TIME_SETTING) || defined(GET_TIME_FROM_PHONE)
time_t manualTimeShift;
#endif

#ifdef GET_TIME_FROM_PHONE
time_t phoneTimeLastSync;
#endif

uint8_t selectedSettings = 0U;

#if USE_BUTTON
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);
#endif

#if USE_OTA
OtaManager otaManager(&showWarning);
OtaPhase OtaManager::OtaFlag = OtaPhase::None;
#endif

#if USE_MQTT
AsyncMqttClient* mqttClient = NULL;
AsyncMqttClient* MqttManager::mqttClient = NULL;
char* MqttManager::clientId = NULL;
char* MqttManager::lampInputBuffer = NULL;
char* MqttManager::topicInput = NULL;
#ifdef PUBLISH_STATE_IN_OLD_FORMAT
char* MqttManager::topicOutput = NULL;
#endif
char* MqttManager::topicOutputJSON = NULL;
bool MqttManager::needToPublish = false;
char MqttManager::mqttBuffer[] = {};
uint32_t MqttManager::mqttLastConnectingAttempt = 0;
SendCurrentDelegate MqttManager::sendCurrentDelegate = NULL;
#endif

WebServer server(80);
WebServer HTTP (ESP_HTTP_PORT);         // Объект для обнавления с web страницы
File fsUploadFile;                       // Для файловой системы

// --- ИНИЦИАЛИЗАЦИЯ ПЕРЕМЕННЫХ -------
uint16_t localPort = ESP_UDP_PORT;
char packetBuffer[MAX_UDP_BUFFER_SIZE];  // buffer to hold incoming packet
char inputBuffer[MAX_UDP_BUFFER_SIZE];

IPAddress getActiveLampIP() {
  if (WiFi.status() == WL_CONNECTED) {
    IPAddress staIp = WiFi.localIP();
    if (staIp != IPAddress(0, 0, 0, 0)) return staIp;
  }

  if (WiFi.getMode() & WIFI_AP) {
    IPAddress apIp = WiFi.softAPIP();
    if (apIp != IPAddress(0, 0, 0, 0)) return apIp;
  }

  return IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], AP_STATIC_IP[3]);
}

String yandexWeatherKey = "";
bool useOpenWeather = false;
String weatherApiKey = "";
String weatherCity = "";
String weatherCityRu = "";
String yandexGeoId = "";
bool   preferYandex = true;
bool actualYandex = true;
float  currentTemp = -999.0f;
String currentCondition = "";
uint32_t weatherUpdateTimer = 0;
const uint32_t WEATHER_UPDATE_INTERVAL = 600000UL;

String findWeatherCityRuByKey(String weatherKey) {
  weatherKey.trim();
  if (!weatherKey.length() || weatherKey == F("|")) weatherKey = F("213|Moscow");

  String citiesJson = readFile(F("weather_city.json"), 16384);
  if (citiesJson == F("Failed") || citiesJson == F("Large") || !citiesJson.length()) return F("");

  DynamicJsonDocument doc(16384);
  DeserializationError err = deserializeJson(doc, citiesJson);
  if (err) return F("");

  String cityRu = doc[weatherKey] | "";
  cityRu.trim();
  if (cityRu.length()) return cityRu;

  int sep = weatherKey.indexOf('|');
  if (sep > 0) {
    String code = weatherKey.substring(0, sep);
    code.trim();
    if (code.length()) {
      JsonObject obj = doc.as<JsonObject>();
      for (JsonPair kv : obj) {
        String key = kv.key().c_str();
        if (key.startsWith(code + F("|"))) {
          cityRu = kv.value() | "";
          cityRu.trim();
          if (cityRu.length()) return cityRu;
        }
      }
    }
  }

  return F("");
}

String normalizeWeatherCityRu(String cityName) {
  cityName.trim();

  static const char* upperRu[] = {
    "А", "Б", "В", "Г", "Д", "Е", "Ё", "Ж", "З", "И", "Й",
    "К", "Л", "М", "Н", "О", "П", "Р", "С", "Т", "У", "Ф",
    "Х", "Ц", "Ч", "Ш", "Щ", "Ъ", "Ы", "Ь", "Э", "Ю", "Я"
  };
  static const char* lowerRu[] = {
    "а", "б", "в", "г", "д", "е", "ё", "ж", "з", "и", "й",
    "к", "л", "м", "н", "о", "п", "р", "с", "т", "у", "ф",
    "х", "ц", "ч", "ш", "щ", "ъ", "ы", "ь", "э", "ю", "я"
  };

  for (uint8_t i = 0; i < 33; i++) cityName.replace(upperRu[i], lowerRu[i]);
  cityName.replace("ё", "е");
  return cityName;
}

bool isValidYandexGeoId(const String& geoId) {
  if (!geoId.length() || geoId.length() > 12 || geoId == F("0")) return false;

  for (size_t i = 0; i < geoId.length(); i++) {
    if (geoId[i] < '0' || geoId[i] > '9') return false;
  }

  return true;
}

bool findWeatherCityByRussianName(String cityRu, String& geoId,
                                  String& cityLatin, String& cityTitle) {
  geoId = F("");
  cityLatin = F("");
  cityTitle = F("");
  String normalizedSearch = normalizeWeatherCityRu(cityRu);
  if (!normalizedSearch.length()) return false;

  String citiesJson = readFile(F("weather_city.json"), 16384);
  if (citiesJson == F("Failed") || citiesJson == F("Large") || !citiesJson.length()) return false;

  DynamicJsonDocument doc(16384);
  DeserializationError err = deserializeJson(doc, citiesJson);
  if (err || !doc.is<JsonObject>()) return false;

  JsonObject cities = doc.as<JsonObject>();
  for (JsonPair city : cities) {
    String savedCityRu = city.value() | "";
    if (normalizeWeatherCityRu(savedCityRu) != normalizedSearch) continue;

    String combinedKey = city.key().c_str();
    int separator = combinedKey.indexOf('|');
    if (separator < 0) continue;

    geoId = combinedKey.substring(0, separator);
    cityLatin = combinedKey.substring(separator + 1);
    cityTitle = savedCityRu;
    geoId.trim();
    cityLatin.trim();
    cityTitle.trim();
    if (isValidYandexGeoId(geoId)) return true;
  }

  geoId = F("");
  cityLatin = F("");
  cityTitle = F("");
  return false;
}

bool findOpenWeatherCityByRussianName(String cityRu, String& cityLatin) {
  String geoId;
  String cityTitle;
  if (!findWeatherCityByRussianName(cityRu, geoId, cityLatin, cityTitle)) return false;
  return cityLatin.length() > 0;
}

void applyWeatherCityValue(String weatherValue) {
  weatherValue.trim();
  if (!weatherValue.length()) weatherValue = F("|");

  int weatherSep = weatherValue.indexOf('|');
  if (weatherSep >= 0) {
    yandexGeoId = weatherValue.substring(0, weatherSep);
    weatherCity = weatherValue.substring(weatherSep + 1);
  } else {
    yandexGeoId = F("");
    weatherCity = weatherValue;
  }

  yandexGeoId.trim();
  weatherCity.trim();

  String lookupKey = weatherValue;
  if (!lookupKey.length() || lookupKey == F("|")) lookupKey = yandexGeoId + F("|") + weatherCity;

  weatherCityRu = findWeatherCityRuByKey(lookupKey);
  if (!weatherCityRu.length()) weatherCityRu = findWeatherCityRuByKey(yandexGeoId + F("|") + weatherCity);
  if (!weatherCityRu.length()) weatherCityRu = weatherCity;
  if (!weatherCityRu.length()) weatherCityRu = F("Москва");
}

String getWeatherCityTitle() {
  String cityTitle = weatherCityRu;
  cityTitle.trim();

  if (!cityTitle.length()) {
    cityTitle = findWeatherCityRuByKey(yandexGeoId + F("|") + weatherCity);
    cityTitle.trim();
  }

  if (!cityTitle.length()) cityTitle = weatherCity;
  if (!cityTitle.length()) cityTitle = F("Москва");
  return cityTitle;
}

bool ssdpInitialized = false;
IPAddress previousIP;
wl_status_t lastWiFiStatus = WL_DISCONNECTED;
uint32_t lastReconnectAttempt = 0;
const uint32_t RECONNECT_INTERVAL = 5000;
const uint32_t ROUTER_BOOT_DELAY = 30000;
uint8_t ESP_CONN_TIMEOUT;

AlarmType alarms[7];
static const uint8_t dawnOffsets[] PROGMEM = {5, 10, 15, 20, 25, 30, 40, 50, 60};   // опции для выпадающего списка параметра "Продолжительность Рассвета" (будильник); синхронизировано с android приложением
uint8_t dawnMode;
uint8_t dawnFlag = 0;
uint32_t thisTime;
bool manualOff = false;

SunsetType sunsets[7];
static const uint8_t sunsetOffsets[] PROGMEM = {5, 10, 15, 20, 25, 30, 40, 50, 60};   // опции для выпадающего списка параметра "Продолжительность Заката";
uint8_t sunsetMode;
uint8_t sunsetFlag = 0;
bool manualsOff = false;

int16_t offset = WIDTH;
uint32_t scrollTimer = 0LL;

uint8_t currentMode;
bool loadingFlag = true;
bool ONflag = false;

#if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)
static inline bool mosfetShouldBeOn()
{
  return ONflag || (dawnFlag == 1 && !manualOff) || (sunsetFlag == 1 && !manualsOff);
}

static inline void updateMosfetState()
{
  digitalWrite(MOSFET_PIN, mosfetShouldBeOn() ? MOSFET_LEVEL : !MOSFET_LEVEL);
}
#endif

//uint32_t eepromTimeout;
//bool settChanged = false;
#if USE_BUTTON
 bool buttonEnabled = true; // Вкл \ откл кнопки
 uint8_t button_type = BUTTON_IS_SENSORY_DEFAULT;   // 0 - механическая, 1 - сенсорная

 uint8_t btn_click_power        = 1U;  // Вкл/выкл лампы
 uint8_t btn_click_next         = 2U;  // Следующий эффект
 uint8_t btn_click_prev         = 3U;  // Предыдущий эффект
 uint8_t btn_click_action4      = 4U;  // OTA / таймер сна
 uint8_t btn_click_ip           = 5U;  // Показать IP
 uint8_t btn_click_time         = 6U;  // Показать время
 uint8_t btn_click_esp_mode     = 7U;  // Переключить WiFi режим
 uint8_t btn_click_sound        = 8U;  // Вкл/выкл звук
 uint8_t btn_click_weather      = 9U;  // Показать погоду

 #if defined(BUTTON_LOCK_ON_START)
  bool buttonBlocing = false;
 #endif
#endif
unsigned char matrixValue[HEIGHT_MAX][WIDTH_MAX]; //это массив для эффекта Огонь

bool TimerManager::TimerRunning = false;
bool TimerManager::TimerHasFired = false;
uint8_t TimerManager::TimerOption = 1U;
uint32_t TimerManager::TimeToFire = 0ULL;

uint8_t FavoritesManager::FavoritesRunning = 0;
uint16_t FavoritesManager::Interval = DEFAULT_FAVORITES_INTERVAL;
uint16_t FavoritesManager::Dispersion = DEFAULT_FAVORITES_DISPERSION;
uint8_t FavoritesManager::UseSavedFavoritesRunning = 0;
uint8_t FavoritesManager::FavoriteModes[MODE_AMOUNT] = {0};
uint32_t FavoritesManager::nextModeAt = 0UL;
bool FavoritesManager::rndCycle = false;

char TextTicker [86];
int Painting = 0; CRGB DriwingColor = CRGB(255, 255, 255);

static uint8_t clampMatrixByte(long value, uint8_t minValue, uint8_t maxValue, uint8_t fallback)
{
  if (value < minValue || value > maxValue) return fallback;
  return (uint8_t)value;
}

static void applyMatrixSegments(uint8_t oneWidth, uint8_t oneHeight, uint8_t countW, uint8_t countH)
{
  segWidth = constrain(oneWidth, WIDTH_MIN, WIDTH_MAX);
  segHeight = constrain(oneHeight, HEIGHT_MIN, HEIGHT_MAX);
  segMatrixW = countW < 1U ? 1U : countW;
  segMatrixH = countH < 1U ? 1U : countH;

  uint16_t totalW = (uint16_t)segWidth * segMatrixW;
  uint16_t totalH = (uint16_t)segHeight * segMatrixH;

  while (segMatrixW > 1U && totalW > WIDTH_MAX)  { segMatrixW--; totalW = (uint16_t)segWidth * segMatrixW; }
  while (segMatrixH > 1U && totalH > HEIGHT_MAX) { segMatrixH--; totalH = (uint16_t)segHeight * segMatrixH; }

  if (totalW < WIDTH_MIN) totalW = WIDTH_MIN;
  if (totalH < HEIGHT_MIN) totalH = HEIGHT_MIN;

  matrixWidth = (uint8_t)totalW;
  matrixHeight = (uint8_t)totalH;
}

static inline bool matrixUseTwoDataLines()
{
  return (NUM_LEDS > LED_2LINES_AFTER_LEDS && ledDataLines == 2U);
}

static inline bool matrixConnectionRotated()
{
  return (ORIENTATION == 1U || ORIENTATION == 3U || ORIENTATION == 5U || ORIENTATION == 7U);
}

static inline uint16_t matrixPhysicalRowLength()
{
  return matrixConnectionRotated() ? (uint16_t)HEIGHT : (uint16_t)WIDTH;
}

static inline uint16_t matrixPhysicalRows()
{
  return matrixConnectionRotated() ? (uint16_t)WIDTH : (uint16_t)HEIGHT;
}

static uint16_t matrixDataSplitIndex()
{
  const uint16_t rowLength = matrixPhysicalRowLength();
  const uint16_t rowsCount = matrixPhysicalRows();
  const uint16_t rowsLine1 = rowsCount / 2U;
  uint16_t splitIndex = rowLength * rowsLine1;

  // Защита от некорректной конфигурации. В норме сюда не попадём.
  if (splitIndex == 0U || splitIndex >= NUM_LEDS)
  {
    splitIndex = NUM_LEDS / 2U;
  }

  return splitIndex;
}

template<EOrder RGB_ORDER>
static void addMatrixLedsForOrder()
{
  if (matrixUseTwoDataLines())
  {
    const uint16_t ledsLine1 = matrixDataSplitIndex();
    const uint16_t ledsLine2 = NUM_LEDS - ledsLine1;

    FastLED.addLeds<WS2812B, LED_PIN,   RGB_ORDER>(&leds[0],         ledsLine1);
    FastLED.addLeds<WS2812B, LED_PIN_2, RGB_ORDER>(&leds[ledsLine1], ledsLine2);

    LOG.printf_P(PSTR("Матрица: %ux%u, %u LED, 2 DATA-линии: GPIO%u = %u LED, GPIO%u = %u LED\n"),
                 WIDTH, HEIGHT, NUM_LEDS, LED_PIN, ledsLine1, LED_PIN_2, ledsLine2);
    LOG.printf_P(PSTR("Разделение DATA по строкам: строка разрыва %u из %u, длина физической строки %u LED\n"),
                 matrixPhysicalRows() / 2U, matrixPhysicalRows(), matrixPhysicalRowLength());
  }
  else
  {
    FastLED.addLeds<WS2812B, LED_PIN, RGB_ORDER>(leds, NUM_LEDS);

    LOG.printf_P(PSTR("Матрица: %ux%u, %u LED, 1 DATA-линия: GPIO%u\n"),
                 WIDTH, HEIGHT, NUM_LEDS, LED_PIN);
  }
}

static void addMatrixLeds()
{
  switch (colorOrder)
  {
    case 0: addMatrixLedsForOrder<RGB>(); break;
    case 1: addMatrixLedsForOrder<RBG>(); break;
    case 2: addMatrixLedsForOrder<GRB>(); break;
    case 3: addMatrixLedsForOrder<GBR>(); break;
    case 4: addMatrixLedsForOrder<BRG>(); break;
    case 5:
    default:
      addMatrixLedsForOrder<BGR>();
      break;
  }
}

//..................... Переменные, добавленные с внедрением web интерфейса .............................................................................................
#define _empty 0x00
#define _dash  0b01000000
#define _deg   0b01100011
#define _C     0b00111001
#define _0     0b00111111
#define _E     0b01111001
#define _F     0b01110001

static float lastLoggedTemp = -999.0;
const uint8_t degreeSymbol[1] = {0b01100011};
const uint8_t cSymbol[1] = {0b00111001};

uint8_t FPSdelay = DYNAMIC;
uint8_t espMode ;
uint8_t random_on;
uint8_t Favorit_only;
//uint32_t my_time;
uint32_t my_timer;
uint8_t time_always;
uint8_t weather_always;
uint8_t show_weather_desc = 1;
bool connect = false;
uint32_t lastResolveTryMoment = 0xFFFFFFFFUL;
uint8_t PRINT_TIME ;
uint16_t PRINT_WEATHER ;

uint8_t tft_on = USE_TFT ? 1 : 0;     // Использовать дисплей TFT: 0 - выкл, 1 - вкл
#if USE_TFT
uint8_t tft_clock_color = 0;
uint8_t tft_weather_color = 1;
uint8_t tft_day_brightness = 100;
uint8_t tft_night_brightness = 10;
bool     tft_ticker_on = false;
uint8_t  tft_ticker_color = 0;
uint16_t tft_ticker_speed = 60;
uint16_t tft_ticker_period = 60;
char     TFTTickerText[128] = {0};
#endif
uint8_t day_night = false;     // если день - true, ночь - false
uint8_t save_file_changes =0;
uint32_t timeout_save_file_changes;
uint8_t first_entry = 0;
uint16_t dawnPosition;
uint16_t sunsetPosition;

#if USE_MULTIPLE_LAMPS_CONTROL
char Host1[16], Host2[16], Host3[16], Host4[16], Host5[16];
uint8_t ml1, ml2, ml3, ml4, ml5;
#endif //USE_MULTIPLE_LAMPS_CONTROL

#if USE_MP3_PLAYER
uint8_t mp3_folder=1;                // Текущая папка для воспроизведения.
uint8_t alarm_sound_on =false;       // Включить/выключить звук будильника
uint8_t sunset_sound_on =false;      // Включить/выключить звук заката
uint8_t alarm_volume;                // Громкость будильника
uint8_t sunset_volume;               // Громкость заката
uint8_t AlarmFolder;                 // Папка будильника
uint8_t SunsetFolder;                // Папка заката
uint8_t Equalizer;                   // Эквалайзер
bool alarm_sound_flag =false;        // Проигрывается ли сейчас будильник
bool sunset_sound_flag =false;       // Проигрывается ли сейчас закат
uint8_t dawnflag_sound = false;      // Звук не начал обслуживание рассвета. Если не true - звук обслуживает рассвет
uint8_t sunsetflag_sound = false;    // Звук не начал обслуживание заката. Если не true - звук обслуживает закат
//uint8_t tmp_fold;
bool advert_flag = false;            // Озвучивается время
bool advert_hour;                    // Озвучиваются часы времени
bool weather_advert_flag = false;    // Озвучивается температура/описание погоды
uint8_t weather_advert_state = 0;    // 0 - стоп, 1 - проигрываются фрагменты, 2 - завершение
uint8_t weather_advert_index = 0;    // Текущий фрагмент озвучки погоды
uint8_t weather_advert_count = 0;    // Количество фрагментов озвучки погоды
uint16_t weather_advert_tracks[5];   // Температура + до 4 файлов описания погоды
uint32_t weather_advert_timer = 0;   // Таймер озвучки температуры
uint8_t day_advert_volume;           // Дневная громкость озвучивания времени/температуры
uint8_t night_advert_volume;         // Ночная громкость озвучивания времени/температуры
bool day_advert_sound_on;            // Вкл.Выкл озвучивания времени днём
bool night_advert_sound_on;          // Вкл.Выкл озвучивания времени ночью
bool alarm_advert_sound_on;          // Вкл.Выкл озвучивания времени будильником
bool day_weather_advert_sound_on;          // Вкл.Выкл озвучивания температуры погоды днём
bool night_weather_advert_sound_on;        // Вкл.Выкл озвучивания температуры погоды ночью
bool alarm_weather_advert_sound_on;        // Вкл.Выкл озвучивания температуры погоды во время будильника
bool day_weather_desc_advert_sound_on;     // Вкл.Выкл озвучивания описания погоды днём
bool night_weather_desc_advert_sound_on;   // Вкл.Выкл озвучивания описания погоды ночью
bool alarm_weather_desc_advert_sound_on;   // Вкл.Выкл озвучивания описания погоды во время будильника
uint8_t mp3_player_connect = 0;      // Плеер не подключен. true - подключен.
uint8_t mp3_player_on = 1;            // Использовать MP3-плеер: 0 - выкл, 1 - вкл
uint8_t mp3_folder_last=255;         // Предыдущая папка для воспроизведения
//uint8_t mp3_folder_change =0;      // Указывает, была ли изменена папка
bool set_mp3_play_now=false;         // Указывает, надо ли играть сейчас мелодии
uint32_t alarm_timer;                // Периодичность проверки и плавного изменения громкости будильника
uint32_t sunset_timer;               // Периодичность проверки и плавного изменения громкости заката
uint32_t mp3_timer = 0;
uint32_t mp3_check_timer = 0;       // Периодическая проверка связи с MP3-плеером
bool mp3_stop = true;                // Озвучка эффектов остановлена
bool pause_on = true;                // Озвучка эффектов на паузе. false - не на паузе
uint8_t eff_volume = 9;              // Громкость воспроизведения
uint8_t eff_sound_on = 0;            // Звук включен - !0 (true), выключен - 0
uint8_t CurrentFolder;               // Папка, на которую переключились (будет проигрываться)
uint8_t CurrentFolder_last = 0;      // Предыдущая текущая папка
HardwareSerial mp3(1);              // Используем UART1
//#ifndef USE_TM1637
// uint8_t minute_tmp;
//#endif
uint8_t mp3_receive_buf[10];
uint8_t effects_folders[MODE_AMOUNT];    // Номера папок для озвучивания
uint16_t ADVERT_TIMER_H, ADVERT_TIMER_M, ADVERT_TIMER_W, ADVERT_TIMER_WDESC; // Продолжительность озвучивания часов, минут, температуры погоды и описания погоды
uint8_t mp3_delay;                       // Задержка между командами проигрывателя
uint8_t send_sound = 1;                  // Передавать или нет сомнительным параметрам звука (папка,озвучивание_on/off,громкость)
uint8_t send_eff_volume = 1;             // Передавать или нет озвучивания_on/off, громкость
#endif  // USE_MP3_PLAYER
#if (USE_TM1637 || USE_TFT)
uint32_t displaySwitchTimer = 0;
uint32_t weatherErrTimer   = 0;
uint32_t weatherErrBlinkTimer = 0;
bool showClock = true;
bool weatherErrActive = false;
bool weatherErrBlinkState = true;

uint32_t CLOCK_SHOW_INTERVAL   = 10000;
uint32_t WEATHER_SHOW_INTERVAL = 5000;
const uint32_t WEATHER_ERR_TIME  = 3000;
const uint32_t WEATHER_ERR_BLINK = 500;

uint32_t DisplayTimer = 0;
uint8_t  LastEffect   = 255;
#if USE_MP3_PLAYER
uint8_t  LastCurrentFolder = 255;
#endif  // USE_MP3_PLAYER
uint8_t  DisplayFlag  = 0;
#endif
uint8_t tm1637_on = USE_TM1637 ? 1 : 0;   // Использовать дисплей TM1637: 0 - выкл, 1 - вкл
#if USE_TM1637
uint8_t DispBrightness = 1;          // +++ Яркость дисплея от 0 до 255(5 уровней яркости с шагом 51). 0 - дисплей погашен 
bool dotFlag = false;                // +++ Флаг: в часах рисуется двоеточие или нет
uint32_t tmr_clock = 0;              // +++ Таймер мигания разделителя часов на дисплее
uint32_t tmr_blink = 0;              // +++ Таймер плавного изменения яркости дисплея
TM1637Display display(CLK, DIO);     // +++ Подключаем дисплей
bool aDirection = false;             // +++ Направление изменения яркости
#endif  //USE_TM1637

#if HEAP_SIZE_PRINT
uint32_t mem_timer;
#endif //HEAP_SIZE_PRINT 

uint8_t ir_on = USE_IR_RECEIVER ? 1 : 0;  // Использовать ИК-приёмник: 0 - выкл, 1 - вкл
#if USE_IR_RECEIVER
 uint32_t IR_Code = 0x00000000;
 uint32_t IR_Repeat_Timer;
 uint32_t IR_Tick_Timer;
 uint8_t IR_Data_Ready;
 uint16_t Enter_Number;
 uint8_t Enter_Digits_Count = 0;
 unsigned long IR_Digit_Timer = 0;
 unsigned long lastIRtime = 0;  // время последнего приёма ИК-сигнала
 
#define IR_REPEAT_TIMER      500   // Время ожидания повтора
#define IR_TICK_TIMER        100    // Время между автоповтором
#define IR_DIGIT_ENTER_TIMER 2000   // Время для ввода второй цифры номера эффекта

 IRrecv irrecv(IR_RECEIVER_PIN);
 decode_results results;
#endif  //USE_IR_RECEIVER

uint8_t RuninTextOverEffects = 0;
uint32_t Last_Time_RuninText = 0;
bool Fill_String = false;
uint8_t SpeedRunningText;
uint8_t ColorRunningText;
uint8_t ColorTextFon;

uint8_t use_static_ip = 0;
IPAddress Static_IP;//(192,168,0,17);  // Статический IP
IPAddress Gateway;//(192,168,0,1);     // Шлюз
IPAddress Subnet;//(255,255,255,0);    // маска подсети
IPAddress DNS1;//(208,67,222,222);     // Серверы DNS. Можно также DNS1(1,1,1,1) или DNS1(8,8,4,4);
IPAddress DNS2(8,8,8,8);               // Резервный DNS

uint8_t C_flag = 0;                    // Служебное
uint16_t white_current_limit;          // Лимит тока для эффекта "Белый свет"
uint16_t current_limit;                // Лимит тока для остальных эффетов

uint32_t getActiveCurrentLimit()
{
  uint32_t limit = (currentMode == EFF_WHITE_COLOR) ? white_current_limit : current_limit;
  if (limit == 0U)
  {
    limit = 0xFFFFUL;
  }
  return limit;
}

void applyCurrentLimitByMode()
{
  static uint32_t lastAppliedLimit = 0UL;
  uint32_t limit = getActiveCurrentLimit();
  if (lastAppliedLimit != limit)
  {
    FastLED.setMaxPowerInVoltsAndMilliamps(5, limit);
    lastAppliedLimit = limit;
  }
}

uint8_t last_minute;                   // Минуты
uint8_t hours;                         // Часы
//uint8_t last_hours; 
uint8_t AutoBrightness;                // Автояркость on/off
uint8_t last_day_night = 0;
bool hasRtc = true;

// Инициализация LittleFS
void FS_init(void) {
  if (!LittleFS.begin()) {
    LOG.println(F("Ошибка монтирования LittleFS!"));
    return;
  }
  HTTP.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm"))
      handleFileRead("/edit.html");
  });
  HTTP.on("/edit", HTTP_PUT, handleFileCreate);
  HTTP.on("/edit", HTTP_DELETE, handleFileDelete);
  HTTP.on("/edit", HTTP_POST, []() {
  HTTP.send(200, "text/plain", "OK");
  }, handleFileUpload);
  HTTP.on("/list", HTTP_GET, handleFileList);
  HTTP.onNotFound([]() {
    if (!handleFileRead(HTTP.uri())) {
      handleFileRead("/index.html");
    }
  });
}

uint8_t rtc_on = USE_RTC ? 1 : 0;     // Использовать RTC: 0 - выкл, 1 - вкл
#if USE_RTC
bool wasError(const char* errorTopic = "")
{
  #ifdef RTC_3231
    uint8_t error = Rtc.LastError();
    if (error != 0){
        LOG.println(errorTopic);
        LOG.println(error);

        switch (error){
        case Rtc_Wire_Error_None:
            LOG.println(F("(none?!)"));
            break;
        case Rtc_Wire_Error_TxBufferOverflow:
            LOG.println(F("transmit buffer overflow"));
            break;
        case Rtc_Wire_Error_NoAddressableDevice:
            LOG.println(F("no device responded"));
            hasRtc = false;
            break;
        case Rtc_Wire_Error_UnsupportedRequest:
            LOG.println(F("device doesn't support request"));
            break;
        case Rtc_Wire_Error_Unspecific:
            LOG.println(F("unspecified error"));
            break;
        case Rtc_Wire_Error_CommunicationTimeout:
            LOG.println(F("communications timed out"));
            hasRtc = false;
            break;
        }
        return true;
    }
#endif
    return false;
}
#endif

void setup()  //==================================================================  void setup()  =========================================================================
{

  Serial.begin(115200);
  delay(300);
  esp_task_wdt_init(8, true);   // Initialize the task watchdog timer

  LOG.print(F("\n\n\nSYSTEM START ESP32-S3\n"));

#if !USE_RTC
  hasRtc = false;
#endif

#if USE_BUTTON
#if defined(BUTTON_LOCK_ON_START) && BUTTON_LOCK_ON_START
  if (button_type) {
    if (digitalRead(BTN_PIN)) buttonEnabled = false;   // сенсорная
  } else {
    if (!digitalRead(BTN_PIN)) buttonEnabled = false;  // механическая
  }
#endif
#endif

  // ПИНЫ
  #ifdef MOSFET_PIN                                         // инициализация пина, управляющего MOSFET транзистором в состояние "выключен"
  pinMode(MOSFET_PIN, OUTPUT);
  #ifdef MOSFET_LEVEL
  digitalWrite(MOSFET_PIN, !MOSFET_LEVEL);
  #endif
  #endif

  #ifdef ALARM_PIN                                          // инициализация пина, управляющего будильником в состояние "выключен"
  pinMode(ALARM_PIN, OUTPUT);
  #ifdef ALARM_LEVEL
  digitalWrite(ALARM_PIN, !ALARM_LEVEL);
  #endif
  #endif
  
   //File Fystem
  #if GENERAL_DEBUG  
  LOG.print(F("\nСтарт файловой системы\n"));
  #endif
  FS_init();  //Запускаем файловую систему

  EEPROM.begin(EEPROM_TOTAL_BYTES_USED);
  delay(10);
  const bool githubOtaRestorePending = EepromManager::IsGitHubOtaRestorePending();

  if (isConfigRestorePending()) {
    bool restored = restoreConfigBackupFromPartition(githubOtaRestorePending);
    clearConfigRestorePending();
    if (restored) LOG.println(F("Настройки восстановлены"));
    else          LOG.println(F("Не удалось восстановить настройки"));
  }

  {
    String configHardware = readFile(F("config_hardware.json"), 1024);
    #if USE_BUTTON
    String buttonOnCfg = jsonRead(configHardware, "button_on");
    buttonEnabled = (buttonOnCfg.length() == 0) ? 1 : buttonOnCfg.toInt();
    String buttonTypeCfg = jsonRead(configHardware, "button_type");
    button_type = (buttonTypeCfg.length() == 0) ? BUTTON_IS_SENSORY_DEFAULT : buttonTypeCfg.toInt();
    #endif
    #if USE_TM1637
    String tm1637OnCfg = jsonRead(configHardware, "tm1637_on");
    tm1637_on = (tm1637OnCfg.length() == 0) ? 1 : tm1637OnCfg.toInt();
    #endif
    #if USE_TFT
    String tftOnCfg = jsonRead(configHardware, "tft_on");
    tft_on = (tftOnCfg.length() == 0) ? 1 : tftOnCfg.toInt();
    #endif
    #if USE_IR_RECEIVER
    String irOnCfg = jsonRead(configHardware, "ir_on");
    ir_on = (irOnCfg.length() == 0) ? 1 : irOnCfg.toInt();
    #endif
    #if USE_RTC
    String rtcOnCfg = jsonRead(configHardware, "rtc_on");
    rtc_on = (rtcOnCfg.length() == 0) ? 1 : rtcOnCfg.toInt();
    #endif
    #if USE_MP3_PLAYER
    String mp3OnCfg = jsonRead(configHardware, "mp3_on");
    mp3_player_on = (mp3OnCfg.length() == 0) ? 1 : mp3OnCfg.toInt();
    #endif
    String matrixWidthCfg = jsonRead(configHardware, "m_w");
    String matrixHeightCfg = jsonRead(configHardware, "m_h");
    String matrixSegWCfg = jsonRead(configHardware, "segMatrix_w");
    String matrixSegHCfg = jsonRead(configHardware, "segMatrix_h");
    String panelFlipCfg = jsonRead(configHardware, "panel_flip");

    uint8_t oneWidth = matrixWidthCfg.length() ? clampMatrixByte(matrixWidthCfg.toInt(), WIDTH_MIN, WIDTH_MAX, WIDTH_DEFAULT) : WIDTH_DEFAULT;
    uint8_t oneHeight = matrixHeightCfg.length() ? clampMatrixByte(matrixHeightCfg.toInt(), HEIGHT_MIN, HEIGHT_MAX, HEIGHT_DEFAULT) : HEIGHT_DEFAULT;
    uint8_t countW = matrixSegWCfg.length() ? constrain(matrixSegWCfg.toInt(), 1, max(1U, (unsigned int)(WIDTH_MAX / oneWidth))) : 1U;
    uint8_t countH = matrixSegHCfg.length() ? constrain(matrixSegHCfg.toInt(), 1, max(1U, (unsigned int)(HEIGHT_MAX / oneHeight))) : 1U;
    panelFlip = (panelFlipCfg.length() && panelFlipCfg.toInt() == 1);
    applyMatrixSegments(oneWidth, oneHeight, countW, countH);

    bool matrixCfgChanged = false;
    if (!matrixWidthCfg.length())  { jsonWrite(configHardware, "m_w", segWidth); matrixCfgChanged = true; }
    if (!matrixHeightCfg.length()) { jsonWrite(configHardware, "m_h", segHeight); matrixCfgChanged = true; }
    if (!matrixSegWCfg.length())   { jsonWrite(configHardware, "segMatrix_w", segMatrixW); matrixCfgChanged = true; }
    if (!matrixSegHCfg.length())   { jsonWrite(configHardware, "segMatrix_h", segMatrixH); matrixCfgChanged = true; }
    if (!panelFlipCfg.length())    { jsonWrite(configHardware, "panel_flip", panelFlip ? 1 : 0); matrixCfgChanged = true; }
    if (matrixCfgChanged) writeFile(F("config_hardware.json"), configHardware);

    String dataLinesCfg = jsonRead(configHardware, "data_lines");
    ledDataLines = (dataLinesCfg.length() && dataLinesCfg.toInt() == 1) ? 1U : 2U;
    if (!dataLinesCfg.length())
    {
      jsonWrite(configHardware, "data_lines", ledDataLines);
      writeFile(F("config_hardware.json"), configHardware);
    }
  }
  #if USE_BUTTON
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
#endif

  // часы
#if USE_TM1637
  if (tm1637_on) {
  LOG.println(F("Старт дисплея TM1637"));
  tmr_clock = millis();                                     // +++ устанавливаем начальное значение счетчика
  display.setBrightness(DispBrightness);                    // +++ яркость дисплея максимальная = 255
  display.displayByte(_empty, _empty, _empty, _empty);      // +++ очистка дисплея
  display.displayByte(_dash, _dash, _dash, _dash);          // +++ отображаем прочерки
  }
#endif

  #if USE_TFT
    if (tft_on) {
    LOG.println(F("Старт дисплея TFT ST7789"));
    TFT_Init();
    tftShowStartText();
    } else {
      TFT_PowerOff();
    }
  #endif

  #if USE_IR_RECEIVER
  IR_LoadConfigFromFile();
  #endif

  configSetup = readFile(F("config.json"), 2048);
  if (configSetup == F("Failed") || configSetup == F("Large")) configSetup = F("{}");
  if (EepromManager::RestoreWifiBackupAfterGitHubOta(configSetup)) {
    saveConfig();
    LOG.println(F("WiFi настройки восстановлены"));
  }
  //Настраиваем и запускаем SSDP интерфейс
  #if GENERAL_DEBUG
  LOG.print(F("Старт SSDP\n"));
  #endif
  
//-----------Инициализируем переменные, хранящиеся в файле config.json--------------
  LAMP_NAME = jsonRead(configSetup, "SSDP");
  AP_NAME = jsonRead(configSetup, "ssidAP");
  AP_PASS = jsonRead(configSetup, "passwordAP");
  #if USE_BLYNK
  blynkToken = jsonRead(configSetup, "blynk_token");
  blynkEnabled = (jsonReadtoInt(configSetup, "use_blynk") != 0);
  #endif
  Favorit_only = jsonReadtoInt(configSetup, "favorit");
  random_on = jsonReadtoInt(configSetup, "random_on");
  espMode = jsonReadtoInt(configSetup, "ESP_mode");
  PRINT_TIME = jsonReadtoInt(configSetup, "print_time");
  PRINT_WEATHER = jsonReadtoInt(configSetup, "print_weather");
  #if USE_TFT
  {
    String configDisplay = readFile(F("config_display.json"), 1024);
    if (configDisplay == F("Failed") || configDisplay == F("Large")) configDisplay = F("{}");
    tft_clock_color = jsonReadtoInt(configDisplay, "tft_clock_color");
    tft_weather_color = jsonReadtoInt(configDisplay, "tft_weather_color");
    bool tftBrightnessConfigChanged = false;
    String tftDayBrightnessCfg = jsonRead(configDisplay, "tft_day_bright");
    String tftNightBrightnessCfg = jsonRead(configDisplay, "tft_night_bright");
    if (tftDayBrightnessCfg.length()) {
      tft_day_brightness = constrain(tftDayBrightnessCfg.toInt(), 1, BRIGHTNESS_PERCENT_MAX);
    } else {
      tft_day_brightness = brightnessByteToPercent(constrain(jsonReadtoInt(configSetup, "day_bright"), 1, 255));
      tftBrightnessConfigChanged = true;
    }
    if (tftNightBrightnessCfg.length()) {
      tft_night_brightness = constrain(tftNightBrightnessCfg.toInt(), 1, BRIGHTNESS_PERCENT_MAX);
    } else {
      tft_night_brightness = brightnessByteToPercent(constrain(jsonReadtoInt(configSetup, "night_bright"), 1, 255));
      tftBrightnessConfigChanged = true;
    }
    if (!tftDayBrightnessCfg.length() || tftDayBrightnessCfg.toInt() != tft_day_brightness) {
      jsonWrite(configDisplay, "tft_day_bright", tft_day_brightness);
      tftBrightnessConfigChanged = true;
    }
    if (!tftNightBrightnessCfg.length() || tftNightBrightnessCfg.toInt() != tft_night_brightness) {
      jsonWrite(configDisplay, "tft_night_bright", tft_night_brightness);
      tftBrightnessConfigChanged = true;
    }
    tft_ticker_on = jsonReadtoInt(configDisplay, "tft_ticker_on");
    tft_ticker_color = jsonReadtoInt(configDisplay, "tft_ticker_color");
    tft_ticker_speed = jsonReadtoInt(configDisplay, "tft_ticker_speed");
    tft_ticker_period = jsonReadtoInt(configDisplay, "tft_ticker_period");
    (jsonRead(configDisplay, "tft_ticker_text")).toCharArray(TFTTickerText, (jsonRead(configDisplay, "tft_ticker_text")).length() + 1);
    if (tftBrightnessConfigChanged) writeFile(F("config_display.json"), configDisplay);
  }
  #endif
  ESP_CONN_TIMEOUT = jsonReadtoInt(configSetup, "TimeOut");
  time_always = jsonReadtoInt(configSetup, "time_always");
  #if USE_WEATHER
  weather_always = jsonReadtoInt(configSetup, "weather_always");
  {
    String showWeatherDescCfg = jsonRead(configSetup, "show_weather_desc");
    show_weather_desc = showWeatherDescCfg.length() ? showWeatherDescCfg.toInt() : 1;
    if (!showWeatherDescCfg.length()) jsonWrite(configSetup, "show_weather_desc", show_weather_desc);
  }
  #endif
  (jsonRead(configSetup, "run_text")).toCharArray (TextTicker, (jsonRead(configSetup, "run_text")).length()+1);
  NIGHT_HOURS_START = 60U * jsonReadtoInt(configSetup, "night_time");
  NIGHT_HOURS_STOP = 60U * jsonReadtoInt(configSetup, "day_time");
  bool dayNightBrightnessConfigChanged = false;
  const bool oldDayNightBrightnessScale = (jsonReadtoInt(configSetup, "brightness_scale_100") != 1);
  const int storedNightBrightness = jsonReadtoInt(configSetup, "night_bright");
  const int storedDayBrightness = jsonReadtoInt(configSetup, "day_bright");
  if (oldDayNightBrightnessScale) {
    NIGHT_HOURS_BRIGHTNESS = brightnessByteToPercent(constrain(storedNightBrightness, 1, 255));
    DAY_HOURS_BRIGHTNESS = brightnessByteToPercent(constrain(storedDayBrightness, 1, 255));
    jsonWrite(configSetup, "brightness_scale_100", 1);
    dayNightBrightnessConfigChanged = true;
  } else {
    NIGHT_HOURS_BRIGHTNESS = constrain(storedNightBrightness, 1, BRIGHTNESS_PERCENT_MAX);
    DAY_HOURS_BRIGHTNESS = constrain(storedDayBrightness, 1, BRIGHTNESS_PERCENT_MAX);
  }
  if (storedNightBrightness != NIGHT_HOURS_BRIGHTNESS) {
    jsonWrite(configSetup, "night_bright", NIGHT_HOURS_BRIGHTNESS);
    dayNightBrightnessConfigChanged = true;
  }
  if (storedDayBrightness != DAY_HOURS_BRIGHTNESS) {
    jsonWrite(configSetup, "day_bright", DAY_HOURS_BRIGHTNESS);
    dayNightBrightnessConfigChanged = true;
  }
  if (dayNightBrightnessConfigChanged) saveConfig();
  DONT_TURN_ON_AFTER_SHUTDOWN = jsonReadtoInt(configSetup, "effect_always"); 
  FavoritesManager::rndCycle = jsonReadtoInt(configSetup, "rnd_cycle");  // Перемешать Цикл
  AUTOMATIC_OFF_TIME = (30UL * 60UL * 1000UL) * ( uint32_t )(jsonReadtoInt(configSetup, "timer5h"));
  RuninTextOverEffects = jsonReadtoInt(configSetup, "toe");  // Выводить бегущую поверх эффектов строку
  SpeedRunningText = jsonReadtoInt(configSetup, "spt");      // Скорость бегущей строки
  ColorRunningText = jsonReadtoInt(configSetup, "sct");      // Цвет бегущей строки
  ColorTextFon = jsonReadtoInt(configSetup, "ctf");          // Выводить бегущую строку на цветном фоне
  AutoBrightness = jsonReadtoInt(configSetup, "auto_bri");   // Автоматическое понижение яркости on/off
  #ifdef USE_NTP
  (jsonRead(configSetup, "ntp")).toCharArray (NTP_ADDRESS, (jsonRead(configSetup, "ntp")).length()+1);
  #endif
  #ifdef USE_NTP
  winterTime.offset = jsonReadtoInt(configSetup, "timezone") * 60;
  summerTime.offset = winterTime.offset + jsonReadtoInt(configSetup, "Summer_Time") *60;
  localTimeZone.setRules (summerTime, winterTime);
  #endif
  
  #if USE_RTC
    if (rtc_on) {
    #ifdef RTC_3231
    Wire.begin(I2C_SDA, I2C_SCL);
    #endif
    Rtc.Begin();
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    time_t utcCompiledUnix = localTimeZone.toUTC(compiled.Epoch32Time());
    RtcDateTime utcCompiled;
    utcCompiled.InitWithEpoch32Time(utcCompiledUnix);
    printDateTime(utcCompiled);

    if (!Rtc.IsDateTimeValid()){
        if (!wasError("setup IsDateTimeValid")){
            LOG.println(F("RTC lost confidence in the DateTime!"));
            Rtc.SetDateTime(utcCompiled);
        }
    }

    if (!Rtc.GetIsRunning()){
        if (!wasError("setup GetIsRunning")){
            LOG.println(F("RTC was not actively running, starting now"));
            Rtc.SetIsRunning(true);
        }
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (!wasError("setup GetDateTime")){
        if (now < utcCompiled){
            LOG.println(F("RTC is older than compile time, updating DateTime"));
            Rtc.SetDateTime(utcCompiled);
        }
        else if (now > utcCompiled){
            LOG.println(F("RTC is newer than compile time, this is expected"));
        }
        else if (now == utcCompiled){
            LOG.println(F("RTC is the same as compile time, while not expected all is still fine"));
        }
    }
    #ifdef RTC_3231
    Rtc.Enable32kHzPin(false);
    wasError("setup Enable32kHzPin");
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
    wasError("setup SetSquareWavePin");
    #endif
    } else {
      hasRtc = false;
    }
  #endif //USE_RTC

  #if USE_MP3_PLAYER
  eff_volume = jsonReadtoInt(configSetup, "vol");
  eff_sound_on = (jsonReadtoInt(configSetup, "on_sound")==0)? 0 : eff_volume;

  String configAlarm = readFile(F("config_alarm.json"), 512);
  String alarmVolumeCfg = jsonRead(configAlarm, "alm_vol");
  alarm_volume = alarmVolumeCfg.length() ? alarmVolumeCfg.toInt() : 10;
  String alarmFolderCfg = jsonRead(configAlarm, "alm_fold");
  AlarmFolder = alarmFolderCfg.length() ? alarmFolderCfg.toInt() : 99;
  String alarmSoundCfg = jsonRead(configAlarm, "on_alm_snd");
  alarm_sound_on = alarmSoundCfg.length() ? alarmSoundCfg.toInt() : 0;

  String configSunset = readFile(F("config_sunset.json"), 512);
  sunset_volume = jsonReadtoInt(configSunset, "sun_vol");
  SunsetFolder = jsonReadtoInt(configSunset, "sun_fold");
  sunset_sound_on = jsonReadtoInt(configSunset, "on_sun_snd");
  day_advert_sound_on = jsonReadtoInt(configSetup,"on_day_adv");
  night_advert_sound_on = jsonReadtoInt(configSetup,"on_night_adv");
  String dayWeatherAdvCfg = jsonRead(configSetup,"on_day_wadv");
  if (dayWeatherAdvCfg.length()) day_weather_advert_sound_on = dayWeatherAdvCfg.toInt();
  else {
    day_weather_advert_sound_on = day_advert_sound_on;
    jsonWrite(configSetup, "on_day_wadv", day_weather_advert_sound_on);
  }
  String nightWeatherAdvCfg = jsonRead(configSetup,"on_night_wadv");
  if (nightWeatherAdvCfg.length()) night_weather_advert_sound_on = nightWeatherAdvCfg.toInt();
  else {
    night_weather_advert_sound_on = night_advert_sound_on;
    jsonWrite(configSetup, "on_night_wadv", night_weather_advert_sound_on);
  }

  String dayWeatherDescCfg = jsonRead(configSetup,"on_day_wdesc");
  if (dayWeatherDescCfg.length()) day_weather_desc_advert_sound_on = dayWeatherDescCfg.toInt();
  else {
    day_weather_desc_advert_sound_on = day_weather_advert_sound_on;
    jsonWrite(configSetup, "on_day_wdesc", day_weather_desc_advert_sound_on);
  }

  String nightWeatherDescCfg = jsonRead(configSetup,"on_night_wdesc");
  if (nightWeatherDescCfg.length()) night_weather_desc_advert_sound_on = nightWeatherDescCfg.toInt();
  else {
    night_weather_desc_advert_sound_on = night_weather_advert_sound_on;
    jsonWrite(configSetup, "on_night_wdesc", night_weather_desc_advert_sound_on);
  }

  day_advert_volume = jsonReadtoInt(configSetup,"day_vol");
  String alarmAdvertCfg = jsonRead(configAlarm,"on_alm_adv");
  alarm_advert_sound_on = alarmAdvertCfg.length() ? alarmAdvertCfg.toInt() : 0;
  String alarmWeatherAdvCfg = jsonRead(configAlarm,"on_alm_wadv");
  alarm_weather_advert_sound_on = alarmWeatherAdvCfg.length() ? alarmWeatherAdvCfg.toInt() : alarm_advert_sound_on;
  String alarmWeatherDescCfg = jsonRead(configAlarm,"on_alm_wdesc");
  alarm_weather_desc_advert_sound_on = alarmWeatherDescCfg.length() ? alarmWeatherDescCfg.toInt() : alarm_weather_advert_sound_on;
  night_advert_volume = jsonReadtoInt(configSetup,"night_vol");
  Equalizer = jsonReadtoInt(configSetup, "eq");
  send_sound = jsonReadtoInt(configSetup, "s_s");
  send_eff_volume = jsonReadtoInt(configSetup, "s_e_v");
  #endif // USE_MP3_PLAYER
  {
  String configHardware = readFile(F("config_hardware.json"), 1024);    
  bool configHardwareChanged = false;
  current_limit = jsonReadtoInt(configHardware, "cur_lim");
  {
    String whiteCurrentLimitCfg = jsonRead(configHardware, "cur_lim_white");
    white_current_limit = whiteCurrentLimitCfg.length() ? whiteCurrentLimitCfg.toInt() : WHITE_COLOR_CURRENT_LIMIT;
    if (white_current_limit > CURRENT_LIMIT) white_current_limit = CURRENT_LIMIT;
    if (!whiteCurrentLimitCfg.length()) {
      jsonWrite(configHardware, "cur_lim_white", white_current_limit);
      configHardwareChanged = true;
    }
  }
  MATRIX_TYPE = jsonReadtoInt(configHardware, "m_t");
  ORIENTATION = jsonReadtoInt(configHardware, "m_o");
  colorOrder = jsonReadtoInt(configHardware, "color_order");
  {
    String dataLinesCfg = jsonRead(configHardware, "data_lines");
    ledDataLines = (dataLinesCfg.length() && dataLinesCfg.toInt() == 1) ? 1U : 2U;
  }
  #if USE_MP3_PLAYER
  ADVERT_TIMER_H = 100 * jsonReadtoInt(configHardware, "tim_h");
  ADVERT_TIMER_M = 100 * jsonReadtoInt(configHardware, "tim_m");
  ADVERT_TIMER_W = 100 * jsonReadtoInt(configHardware, "tim_w");
  {
    String weatherDescTimerCfg = jsonRead(configHardware, "tim_wdesc");
    ADVERT_TIMER_WDESC = 100 * (weatherDescTimerCfg.length() ? weatherDescTimerCfg.toInt() : jsonReadtoInt(configHardware, "tim_w"));
  }
  mp3_delay = 10 * jsonReadtoInt(configHardware, "delay");
  #endif
  #if USE_BUTTON
  {
    String s;
    s = jsonRead(configHardware, "btn_click_power");
    btn_click_power = s.length() ? s.toInt() : 1U;
    s = jsonRead(configHardware, "btn_click_next");
    btn_click_next = s.length() ? s.toInt() : 2U;
    s = jsonRead(configHardware, "btn_click_prev");
    btn_click_prev = s.length() ? s.toInt() : 3U;
    s = jsonRead(configHardware, "btn_click_action4");
    btn_click_action4 = s.length() ? s.toInt() : 4U;
    s = jsonRead(configHardware, "btn_click_ip");
    btn_click_ip = s.length() ? s.toInt() : 5U;
    s = jsonRead(configHardware, "btn_click_time");
    btn_click_time = s.length() ? s.toInt() : 6U;
    s = jsonRead(configHardware, "btn_click_esp_mode");
    btn_click_esp_mode = s.length() ? s.toInt() : 7U;
    s = jsonRead(configHardware, "btn_click_sound");
    btn_click_sound = s.length() ? s.toInt() : 8U;
    s = jsonRead(configHardware, "btn_click_weather");
    btn_click_weather = s.length() ? s.toInt() : 9U;
  }
#endif
    offset = WIDTH;
    if (configHardwareChanged) writeFile(F("config_hardware.json"), configHardware);
  }
  {
  String configIP = readFile(F("config_ip.json"), 512);
  use_static_ip = jsonReadtoInt(configSetup, "s_IP");
  Static_IP.fromString(jsonRead(configIP, "ip"));
  Gateway.fromString(jsonRead(configIP, "gateway"));
  Subnet.fromString(jsonRead(configIP, "subnet"));
  DNS1.fromString(jsonRead(configIP, "dns"));
  }

  // TELNET
  #if defined(GENERAL_DEBUG) && GENERAL_DEBUG_TELNET
  telnetServer.begin();
  for (uint8_t i = 0; i < 100; i++)                         // пауза 10 секунд в отладочном режиме, чтобы успеть подключиться по протоколу telnet до вывода первых сообщений
  {
    handleTelnetClient();
    delay(100);
    esp_task_wdt_reset();
  }
  #endif


  // ЛЕНТА/МАТРИЦА
  addMatrixLeds();
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(BINARY_DITHER);   // более мягкие переходы на низкой яркости
  applyCurrentLimitByMode();
  FastLED.clear();
  FastLED.show();

#ifdef USE_SHUFFLE_FAVORITES // первоначальная очередь избранного до перемешивания
    for (uint8_t i = 0; i < MODE_AMOUNT; i++)
      shuffleFavoriteModes[i] = i;
#endif

  // Временный массив нужен только для полной проверки effect.ini и как
  // источник начальных значений, если в EEPROM ещё нет исправных настроек.
  // После выхода из блока память массива освобождается.
  {
    ModeType parsedDefaults[MODE_AMOUNT];
    const bool defaultsValid = readEffectSettingsFromFile(parsedDefaults);
    if (!defaultsValid)
    {
      EepromManager::FillSafeSettings(parsedDefaults);
      LOG.println(F("Повреждён /effect.ini; подготовлены безопасные значения"));
    }
    EepromManager::InitEepromSettings(modes, parsedDefaults, defaultsValid);
  }
  effectSettingsDirty = false;
    

  if(DONT_TURN_ON_AFTER_SHUTDOWN){
      ONflag = false;
  jsonWrite(configSetup, "Power", ONflag);
  }
  else
      ONflag = jsonReadtoInt(configSetup, "Power");  // Состояние питания хранится в config.json
  const int storedMode = jsonReadtoInt(configSetup, "eff_sel");
  currentMode = (storedMode >= 0 && storedMode < MODE_AMOUNT) ? (uint8_t)storedMode : 0U;
  jsonWrite(configSetup, "eff_sel", currentMode);
  syncCurrentEffectToConfig();
  first_entry = 1;
  handle_alarm ();
  handle_sunset ();
  first_entry = 0;
  FavoritesManager::FavoritesRunning = jsonReadtoInt(configSetup, "cycle_on");  // Чтение состояния настроек режима Цикл 
  FavoritesManager::Interval = jsonReadtoInt(configSetup, "time_eff");          // Вкл/выкл,время переключения, дисперсия, вкл цикла после перезагрузки
  FavoritesManager::Dispersion = jsonReadtoInt(configSetup, "disp");
  FavoritesManager::UseSavedFavoritesRunning = jsonReadtoInt(configSetup, "cycle_allwase");
  jsonWrite(configSetup, "tmr", 0);
  first_entry = 1;
  handle_cycle_set();  // Чтение выбранных эффектов
  first_entry = 0;
#if USE_MP3_PLAYER
  first_entry = 1;
  handle_sound_set();  //Чтение выбранных папок
  first_entry = 0;
#endif  // USE_MP3_PLAYER
#if USE_MULTIPLE_LAMPS_CONTROL  
  multilamp_get ();   // Чтение из файла адресов синхронно управляемых ламп 
#endif //USE_MULTIPLE_LAMPS_CONTROL
  
  // MP3 Player
   
  #if USE_MP3_PLAYER
   mp3.begin(9600, SERIAL_8N1, MP3_RX_PIN, MP3_TX_PIN);
   LOG.println (F("\nСтарт MP3 Player"));
   mp3_timer = millis();
   mp3_player_connect = 1;
  #endif 

  // WI-FI
  LOG.printf_P(PSTR("\nРабочий режим лампы: ESP_MODE = %d\n"), espMode);
  WiFi.persistent(false);   // Побережём EEPROM
  WiFi.mode(espMode == 0U ? WIFI_AP : WIFI_STA);

  if (espMode == 0U) {
    // Режим точки доступа
    if (sizeof(AP_STATIC_IP))
    {
      WiFi.softAPConfig(                      
        IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], AP_STATIC_IP[3]),      // IP адрес WiFi точки доступа
        IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], 1),                    // первый доступный IP адрес сети
        IPAddress(255, 255, 255, 0));                                                       // маска подсети
    }
     WiFi.softAP(AP_NAME.c_str(), AP_PASS.c_str());
    delay(100);
    LOG.print(F("AP запущен: "));
    LOG.println(WiFi.softAPIP());
    #if USE_TFT
    TFT_ShowIP(WiFi.softAPIP().toString().c_str());
    #endif
    connect = true;
    #if DISPLAY_IP_AT_START
        loadingFlag = true;
      #if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)      // установка сигнала в пин, управляющий MOSFET транзистором, матрица должна быть включена на время вывода текста
        digitalWrite(MOSFET_PIN, MOSFET_LEVEL);
      #endif
        while(!fillString(WiFi.softAPIP().toString().c_str(), CRGB::White, false)) {
           delay(1);
            esp_task_wdt_reset();
           }
        if (ColorTextFon  & (!ONflag || (currentMode == EFF_COLOR && modes[currentMode].Scale < 3))){
          FastLED.clear();
          delay(1);
          FastLED.show();
        }
      #if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)      // возвращаем MOSFET в состояние лампы/рассвета/заката
        updateMosfetState();
      #endif
        loadingFlag = true;
      #endif  // DISPLAY_IP_AT_START
      #if USE_TFT
      TFT_HideIP();
#endif
      delay (100);
  } else {
    // Режим клиента
    String main_ssid = jsonRead(configSetup, "ssid");
    String main_pass = jsonRead(configSetup, "password");
    if (main_ssid.length() == 0) {
      LOG.println(F("SSID не задан → переход в AP"));
      WiFi.mode(WIFI_AP);
      if (sizeof(AP_STATIC_IP)) {
        WiFi.softAPConfig(
          IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], AP_STATIC_IP[3]),
          IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], 1),
          IPAddress(255, 255, 255, 0));
      }
      WiFi.softAP(AP_NAME.c_str(), AP_PASS.c_str());
      delay(100);
      LOG.print(F("AP запущен: "));
      LOG.println(WiFi.softAPIP());
      connect = false;
      apFallbackActive = true;
      apFallbackStartMs = millis();
      lastRouterRetryMs = millis();
    } else {
      WiFi.mode(WIFI_STA);

      if (use_static_ip) {  
        WiFi.config(Static_IP, Gateway, Subnet, DNS1, DNS2);
      }

      wifiMulti.addAP(main_ssid.c_str(), main_pass.c_str());

      delay(10);

      #if USE_BLYNK
      blynkConfiguredEnabled = blynkEnabled;
      blynkConfiguredToken   = blynkToken;
      blynkConfigured        = (blynkEnabled && blynkToken.length() > 0);
      if (blynkConfigured) {
        Blynk.config(blynkToken.c_str(), "blynk.tk", 8080);
      }
      #endif

      esp_task_wdt_reset();

      bool wifi_multi_enabled = jsonReadtoInt(configSetup, "wifi_multi");
      if (wifi_multi_enabled) {
        String ssid2 = jsonRead(configSetup, "ssid2");
        String pass2 = jsonRead(configSetup, "password2");
        String ssid3 = jsonRead(configSetup, "ssid3");
        String pass3 = jsonRead(configSetup, "password3");

        if (ssid2.length() > 0) {
          wifiMulti.addAP(ssid2.c_str(), pass2.c_str());
          LOG.printf_P(PSTR("Добавлена сеть 2: %s\n"), ssid2.c_str());
        }
        if (ssid3.length() > 0) {
          wifiMulti.addAP(ssid3.c_str(), pass3.c_str());
          LOG.printf_P(PSTR("Добавлена сеть 3: %s\n"), ssid3.c_str());
        }
      }

      LOG.print(F("Подключение к WiFi"));
      uint32_t startTime = millis();
      uint32_t timeout = (uint32_t)ESP_CONN_TIMEOUT * 1000UL;
      if (timeout == 0) timeout = 15000UL;

      while (wifiMulti.run() != WL_CONNECTED) {
        delay(500);
        LOG.print(F("."));

        esp_task_wdt_reset();

        if (millis() - startTime > timeout) {
          LOG.println(F("\nНе удалось подключиться, запускаем временный AP"));
          WiFi.mode(WIFI_AP_STA);
          if (sizeof(AP_STATIC_IP)) {
            WiFi.softAPConfig(
              IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], AP_STATIC_IP[3]),
              IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], 1),
              IPAddress(255, 255, 255, 0));
          }
          WiFi.softAP(AP_NAME.c_str(), AP_PASS.c_str());
          delay(100);
          LOG.print(F("AP запущен: "));
          LOG.println(WiFi.softAPIP());

          connect = false;
          apFallbackActive = true;
          apFallbackStartMs = millis();
          lastRouterRetryMs = millis();
          break;
        }
      }

      if (WiFi.status() == WL_CONNECTED) {
        LOG.println(F("\nWiFi подключён!"));
        LOG.print(F("SSID: ")); LOG.println(WiFi.SSID());
        LOG.print(F("IP: ")); LOG.println(WiFi.localIP());
        connect = true;
        apFallbackActive = false;
      }
    }
      String startupIp = getActiveLampIP().toString();
      #if USE_TFT
      TFT_ShowIP(startupIp.c_str());
      #endif
    LOG.print(F("RSSI: ")); LOG.print(WiFi.RSSI()); LOG.println(F(" dBm"));
    // После неудачного подключения временная AP уже поднята, но STA ещё не
    // подключена. Не помечаем это как успешное соединение с роутером.
    connect = (WiFi.status() == WL_CONNECTED);
    #if DISPLAY_IP_AT_START
        loadingFlag = true;
      #if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)      // установка сигнала в пин, управляющий MOSFET транзистором, матрица должна быть включена на время вывода текста
        digitalWrite(MOSFET_PIN, MOSFET_LEVEL);
      #endif
        while(!fillString(startupIp.c_str(), CRGB::White, false)) {
           delay(1);
            esp_task_wdt_reset();
           }
        if (ColorTextFon  & (!ONflag || (currentMode == EFF_COLOR && modes[currentMode].Scale < 3))){
          FastLED.clear();
          delay(1);
          FastLED.show();
        }
      #if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)      // возвращаем MOSFET в состояние лампы/рассвета/заката
        updateMosfetState();
      #endif
        loadingFlag = true;
      #endif  // DISPLAY_IP_AT_START
      #if USE_TFT
      TFT_HideIP();
      #endif
  }
  if (WiFi.status() == WL_CONNECTED && WiFi.getMode() == WIFI_STA) {
    SSDP_init();
    ssdpInitialized = true;
  }

  // UDP 
  LOG.printf_P(PSTR("\nСтарт UDP сервера. Порт: %u\n"), localPort);
  Udp.begin(localPort);

  //Настраиваем и запускаем HTTP интерфейс
  User_setings ();
  #if GENERAL_DEBUG
  LOG.print (F("Старт WebServer\n"));
  #endif
  HTTP_init();
  WiFiClient client;  //Declare an object of class HTTPClient
 
 // NTP
  #ifdef USE_NTP
  timeClient.begin();
    esp_task_wdt_reset();
  #endif


  // MQTT
  #if USE_MQTT
  String configMQTT = readFile(F("config_mqtt.json"), 512);
  String str;
  if(!MqttServer.fromString(jsonRead(configMQTT, "mq_ip"))){
        jsonRead(configMQTT, "mq_ip").toCharArray(MqttHost, jsonRead(configMQTT, "mq_ip").length()+1);
        mqttIPaddr = false;
/*        WiFi.hostByName(MqttHost, MqttServer);
*/
  }
  else
      mqttIPaddr = true;
  str = jsonRead(configMQTT, "mq_user");
  str.toCharArray(MqttUser, str.length()+1);
  str = jsonRead(configMQTT, "mq_pass");
  str.toCharArray(MqttPassword, str.length()+1);
  str = jsonRead(configMQTT, "topic");
  str.toCharArray(TopicBase, str.length()+1);
  MqttPort = jsonReadtoInt(configMQTT, "mq_port");
  MqttOn = jsonReadtoInt(configMQTT, "mq_on");
  MqttPeriod = jsonReadtoInt(configMQTT, "mq_prd");
  #if GENERAL_DEBUG
   LOG. println("Start MQTT");
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
  #endif //GENERAL_DEBUG

  if (espMode == 1U)
  {
    mqttClient = new AsyncMqttClient();
    MqttManager::setupMqtt(mqttClient, inputBuffer, &sendCurrent);    // создание экземпляров объектов для работы с MQTT, их инициализация и подключение к MQTT брокеру
  }
    esp_task_wdt_reset();
  #endif


  // ОСТАЛЬНОЕ
  memset(matrixValue, 0, sizeof(matrixValue)); //это массив для эффекта Огонь. странно, что его нужно залить нулями
  randomSeed(micros());
  changePower();
  
  //IR receiver
  #if USE_IR_RECEIVER
    if (ir_on) irrecv.enableIRIn();  // Start the IR receiver
    IR_Tick_Timer = millis();
    IR_Repeat_Timer = millis();
  #endif  //USE_IR_RECEIVER

  //TM1637 || TFT
#if (USE_TM1637 || USE_TFT)
    String configDisplay = readFile(F("config_display.json"), 1024);
    if (configDisplay == F("Failed") || configDisplay == F("Large")) configDisplay = F("{}");
    uint32_t tmpClock   = jsonReadtoInt(configDisplay, "clock_time");
    uint32_t tmpWeather = jsonReadtoInt(configDisplay, "weather_time");
    if (tmpClock < 3)   tmpClock = 10;
    if (tmpWeather < 3) tmpWeather = 5;
    CLOCK_SHOW_INTERVAL   = tmpClock * 1000UL;
    WEATHER_SHOW_INTERVAL = tmpWeather * 1000UL;
#endif

  // ПОГОДА
#if (USE_WEATHER == 1)
  weatherApiKey = jsonRead(configSetup, "openweather_key");
  bool weatherCfgChanged = false;
  String weatherCityAll = jsonRead(configSetup, "weather_city");
  if (weatherCityAll.length() == 0) {
    weatherCityAll = "|";
    jsonWrite(configSetup, "weather_city", weatherCityAll);
    weatherCfgChanged = true;
  }

  applyWeatherCityValue(weatherCityAll);

  if (!jsonRead(configSetup, "weather_source").length()) {
    jsonWrite(configSetup, "weather_source", 0);
    weatherCfgChanged = true;
  }
  if (weatherCfgChanged) saveConfig();
  String configHardwareWeather = readFile(F("config_hardware.json"), 1024);
  if (configHardwareWeather == F("Failed") || configHardwareWeather == F("Large")) configHardwareWeather = F("{}");
  String weatherOnCfg = jsonRead(configHardwareWeather, "show_weather");
  inClockWeatherMode = (weatherOnCfg.length() == 0) ? 1 : weatherOnCfg.toInt();
  preferYandex = (jsonReadtoInt(configSetup, "weather_source") == 0);
  if (inClockWeatherMode) weatherUpdateTimer = millis() - WEATHER_UPDATE_INTERVAL + 10000;
#endif

  #if USE_TM1637
  DisplayTimer = millis();
#endif
  my_timer=millis();
  
  #if HEAP_SIZE_PRINT
   mem_timer = millis();
  #endif //HEAP_SIZE_PRINT 
}

timerMinim apFallbackTimer(1000UL);

void checkWiFiFallback() {
  if (espMode == 0U) {
    apFallbackActive = false;
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (apFallbackActive) {
      WiFi.softAPdisconnect(true);
      WiFi.mode(WIFI_STA);
      LOG.println(F("WiFi восстановлен"));
    }
    apFallbackActive = false;
    return;
  }

  // Wi-Fi потерян поднимаем AP
  if (!apFallbackActive) {
    if (apFallbackStartMs == 0) {
      apFallbackStartMs = millis();
    }

    if (millis() - apFallbackStartMs >= 20000UL) {
      LOG.println(F("\nWi-Fi потерян"));
      WiFi.mode(WIFI_AP_STA);
      if (sizeof(AP_STATIC_IP)) {
        WiFi.softAPConfig(
          IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], AP_STATIC_IP[3]),
          IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], 1),
          IPAddress(255, 255, 255, 0));
      }
      WiFi.softAP(AP_NAME.c_str(), AP_PASS.c_str());
      delay(100);
      LOG.print(F("Точка доступа запущена: "));
      LOG.println(WiFi.softAPIP());

      apFallbackActive = true;
      lastRouterRetryMs = millis();
      restartSSDP();
    }
  }
}

void wifiReconnect() {
  bool currentlyConnected = (WiFi.status() == WL_CONNECTED);

  if (currentlyConnected && !connect) {
    LOG.println(F("\nWiFi подключён!"));
    LOG.print(F("SSID: ")); LOG.println(WiFi.SSID());
    LOG.print(F("IP:   ")); LOG.println(WiFi.localIP());
    connect = true;
    apFallbackStartMs = 0;
    apFallbackActive = false;
    restartSSDP();
    return;
  }

  if (!currentlyConnected && connect && espMode == 1U) {
    connect = false;
    ssdpInitialized = false;
    apFallbackStartMs = millis();
  }

  // В режиме router каждые 5 минут снова пробуем подключиться к роутеру
  if (espMode == 1U && !currentlyConnected) {
    if (millis() - lastRouterRetryMs >= 300000UL) {
      lastRouterRetryMs = millis();
      LOG.println(F("Повторная попытка подключения к роутеру..."));
      wifiMulti.run();
    }
  }
}

void restartSSDP() {
  if (WiFi.status() != WL_CONNECTED || WiFi.getMode() != WIFI_STA) {
    ssdpInitialized = false;
    return;
  }
  LOG.println(F("Инициализация SSDP..."));
  SSDP_init();
  ssdpInitialized = true;
  previousIP = (espMode == 0U && WiFi.status() != WL_CONNECTED) ? WiFi.softAPIP() : WiFi.localIP();
}

void loop()  //====================================================================  void loop()  ===========================================================================
{
  parseUDP();

  #if USE_RTC
     if (rtc_on && hasRtc) {
      #ifdef RTC_3231
  if (!Rtc.IsDateTimeValid()) {
#else
  {
#endif
    if (!wasError("loop IsDateTimeValid")) {
      LOG.println(F("RTC lost confidence in the DateTime!"));
    }
  }
 }
  #endif //USE_RTC

do {    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++========= Главный цикл ==========+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  checkWiFiFallback();
  wifiReconnect();
  
#if (USE_WEATHER == 1)  // Погода обновляется раз в 10 минут
    if (inClockWeatherMode && WiFi.status() == WL_CONNECTED &&
        millis() - weatherUpdateTimer >= WEATHER_UPDATE_INTERVAL) {
      updateWeather();
      weatherUpdateTimer = millis();
    }
#endif

// Если не устойчивое подключение к WiFi, или не создаётся точка доступа, или лампа не хочет подключаться к вашей сети или вы не можете подключиться к точке доступа, то может быть у вас не качественная плата.
  delay (0);   //Для некоторых плат ( особенно без металлического экрана над ESP и Flash памятью ) эта задержка должна быть увеличена. Подбирается индивидуально в пределах 1-12 мс до устойчивой работы WiFi. Чем меньше, тем лучше. Качественные платы работают с задержкой 0.
  yield();
  
    //if ((connect || !espMode)&&((millis() - my_timer) >= 10UL)) 
    {
    HTTP.handleClient(); // Обработка запросов web страницы. 
    //my_timer = millis();
    }
 
  parseUDP();
  yield();

  if (Painting == 0) {
    effectsTick();
  }
  
  #if USE_TM1637
     if (tm1637_on && millis() - tmr_clock > 500UL) {         // каждую секунду изменяем
      tmr_clock = millis();                     // обновляем значение счетчика
      boolean points[4] = {0,0,0,0};
      if (!DisplayFlag && (!inClockWeatherMode || showClock)) {
      dotFlag = !dotFlag;                       // инверсия флага
      points[1] = dotFlag;
      } else {
      dotFlag = false;
      points[1] = 0;
   }
      if (!DisplayFlag) display.setSegmentPoints(points); // выкл/выкл двоеточия 
      Display_Timer();
   }
    if (tm1637_on && (dawnFlag == 1 || sunsetFlag == 1)) {
      clockTicker_blink();
    }
  #endif  //USE_TM1637

  #if USE_TFT
    if (tft_on) TFT_LoopTick();
  #endif
  
  #if USE_MP3_PLAYER
  if (mp3_player_on) {
    switch (mp3_player_connect){
      case 0:
      case 5:
              if (millis() - mp3_timer > 30000UL) {
                mp3_timer = millis();
                mp3_player_connect = 1;
              }
              break;
      case 1: read_command(1);
              if ((millis() - mp3_timer > 3000UL) || mp3_receive_buf[3] == 0x3F){
                 first_entry = 5;
                 //mp3_timer = millis();
                 mp3_setup ();
                }
              break;
      case 2: read_command(1);
              if ( millis() - mp3_timer > 3000UL || mp3_receive_buf[3] == 0x3F) mp3_player_connect = 3;
              break;
      case 3: mp3_setup(); break;
      case 4:
              mp3_periodic_check();
              if (mp3_player_connect == 4) mp3_loop();
              break;
    }
  }
                    
  #endif

 if (Painting == 0) {
     
  #if HEAP_SIZE_PRINT
   if (millis() - mem_timer > 10000UL) {
       mem_timer = millis();
       LOG.print (F("Heap Size = "));
       LOG.println(ESP.getFreeHeap());
   }
  #endif //HEAP_SIZE_PRINT
  
  #if USE_IR_RECEIVER
      if (ir_on && irrecv.decode(&results)) {
        lastIRtime = millis();
        IR_Code = results.value;
        IR_Data_Ready = 1;
        irrecv.resume();
        }

      if (ir_on && millis() - IR_Tick_Timer > 100) {
        IR_Tick_Timer = millis();
        if (IR_Data_Ready) {
          IR_Receive_Button_Handle();
          IR_Data_Ready = 0;
        }
      }
      if (ir_on && Enter_Digits_Count > 0 &&
        millis() - IR_Digit_Timer > IR_DIGIT_ENTER_TIMER) {
        Apply_Entered_Effect();
      }
  #endif  //USE_IR_RECEIVER

  handleEffectSettingsPersistence();
    yield();

  //#ifdef USE_NTP
  #if defined(USE_NTP) || defined(USE_MANUAL_TIME_SETTING) || defined(GET_TIME_FROM_PHONE)
    timeTick();
  #endif

  #if USE_BUTTON
    buttonTick();
  #endif

  #if USE_OTA
  otaManager.HandleOtaUpdate();                             // ожидание и обработка команды на обновление прошивки по воздуху
  #endif
  OtaPackageHandle();
                                                            
  TimerManager::HandleTimer(&ONflag, &changePower); // обработка таймера отключения лампы
  
  if (FavoritesManager::HandleFavorites(                    // обработка режима избранных эффектов
      &ONflag,
      &currentMode,
      &loadingFlag
      #if defined(USE_NTP) || defined(USE_MANUAL_TIME_SETTING) || defined(GET_TIME_FROM_PHONE)
      , &dawnFlag
      , &sunsetFlag
      #endif
      , &random_on
      , &selectedSettings
      ,espMode
      ))
  {
    #if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
    if (selectedSettings) applyPendingRandomEffectSettings();
    #endif
    #if USE_BLYNK
    updateRemoteBlynkParams();
    #endif
    SetBrightness(modes[currentMode].Brightness);
  }

  #if USE_MQTT
  if (espMode == 1U && mqttClient && WiFi.isConnected() && !mqttClient->connected() && MqttOn)
  {
    MqttManager::mqttConnect();                             // библиотека не умеет восстанавливать соединение в случае потери подключения к MQTT брокеру, нужно управлять этим явно
    MqttManager::needToPublish = true;
  }

  if ((MqttManager::needToPublish || (MqttPeriod && (millis() - mqtt_timer) >= (MqttPeriod * 1000))) && MqttOn)
  //if ((MqttManager::needToPublish ) && MqttOn)
  {
    mqtt_timer = millis();
    if (strlen(inputBuffer) > 0)                            // проверка входящего MQTT сообщения; если оно не пустое - выполнение команды из него и формирование MQTT ответа
    {
      processInputBuffer(inputBuffer, MqttManager::mqttBuffer, true, sizeof(MqttManager::mqttBuffer));
    }
  #ifdef PUBLISH_STATE_IN_OLD_FORMAT  
    MqttManager::publishState(0);  //публикация буфера MQTT ответа в топик <TopicBase>/LedLamp_<ChipId>/state
  #endif // PUBLISH_STATE_IN_OLD_FORMAT
    String MqttSnd = "{\"power\":\"ON\"}"; //Строка для ответа "{"power":"ON","cycle":"OFF","effect":"111","bri":"15","spd":"33","sca":"58","sound":"ON","volume":"10","runt":"10","runc":"123","runf":"1","runc":"220","rnde":"0","rndc":"1","rndf":"0","tmr":59900"}"
    jsonWrite(MqttSnd, "power", ONflag ? "ON" : "OFF");   // Создание строки для MQTT ответа в формате JSON в виде выше.
    jsonWrite(MqttSnd, "cycle", FavoritesManager::FavoritesRunning ? "ON" : "OFF"); // Включить/выключить режим "цикл" ("избранное")
    jsonWrite(MqttSnd, "effect", (String)currentMode);
    jsonWrite(MqttSnd, "bri", (String)modes[currentMode].Brightness);   // Яркость эффектов
    jsonWrite(MqttSnd, "spd", (String)modes[currentMode].Speed);        // Скорость эффектов
    jsonWrite(MqttSnd, "sca", (String)modes[currentMode].Scale);        // Масштаб эффектов
    #if USE_MP3_PLAYER
    jsonWrite(MqttSnd, "sound", eff_sound_on ? "ON" : "OFF");    // Включить/выключить озвучивание эффектов
    jsonWrite(MqttSnd, "vol", (String)eff_volume);                  // Громкость
    #endif //USE_MP3_PLAYER
    jsonWrite(MqttSnd, "runt", (String)RuninTextOverEffects);           // Периодичность ввода бегущей строки
    jsonWrite(MqttSnd, "runc", (String)ColorRunningText);               // Цвет бегущей строки
    jsonWrite(MqttSnd, "runf", (String)ColorTextFon);                   // Фон бегущей строки. 0-черный фон; 1-цветный фон
    jsonWrite(MqttSnd, "runs", (String)SpeedRunningText);               // Скорость бегущей строки
    jsonWrite(MqttSnd, "rnde", (String)FavoritesManager::rndCycle);     // Случайный выбор эффектов в цикле
    jsonWrite(MqttSnd, "rndс", (String)random_on);                      // Случайный выбор настроек эффектов в цикле
    jsonWrite(MqttSnd, "rndf", (String)selectedSettings);               // Случайный выбор настроек текущего эффекта
    uint32_t temp = TimerManager::TimeToFire - millis();
    if (temp && TimerManager::TimerRunning)
        jsonWrite(MqttSnd, "tmr", (String)(temp / 1000));               // Количество секунд до срабатывания таймера
    else
        jsonWrite(MqttSnd, "tmr", (String)0);                           // Количество секунд до срабатывания таймера == 0
    
    MqttSnd.toCharArray(MqttManager::mqttBuffer, sizeof(MqttManager::mqttBuffer)); // безопасно ограничиваем ответ MQTT-буфером
    MqttManager::publishState(1);  //публикация буфера MQTT ответы (JSON): "{"power":"ON","cycle":"OFF","effect":"111","bri":"15","spd":"33","sca":"58","sound":"ON","volume":"10","runt":"10","runc":"123","runf":"1","runc":"220","rnde":"0","rndc":"1","rndf":"0","tmr":59900"}" в топик <TopicBase>/LedLamp_<ChipId>/snd
  }
  #endif

  #if USE_BLYNK
  if (blynkConfigured && espMode == 1U && WiFi.isConnected())
    Blynk.run();
  #endif

  #if defined(GENERAL_DEBUG) && GENERAL_DEBUG_TELNET
  handleTelnetClient();
  #endif
 } //if (Painting == 0)
  yield();
    esp_task_wdt_reset();
} while (connect);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

#if USE_RTC
void printDateTime(const RtcDateTime& dt)
{
    char datestring[26];

    snprintf_P(datestring,
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Day(),
            dt.Month(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    LOG.println(datestring);
}
#endif

// ==================================================================== ПОГОДА ==================================================================

#if (USE_WEATHER == 1)

static inline String toLowerCopy(String s) { s.toLowerCase(); return s; }

String tempToRussianFeeling(float t) {
  if (t <= -25) return "экстремальный мороз";
  if (t <= -15) return "сильный мороз";
  if (t <= -7)  return "мороз";
  if (t <= 0)   return "морозно";
  if (t <= 5)   return "холодно";
  if (t <= 12)  return "прохладно";
  if (t <= 18)  return "комфортно";
  if (t <= 24)  return "тепло";
  if (t <= 30)  return "жарко";
  return "очень жарко";
}

String normalizeCond(String s) {
  s.trim();
  if (!s.length()) return "";

  String low = toLowerCopy(s);
  low.replace('_', '-');
  low.replace(' ', '-');

  if (low == "overcast-thunderstorms-with-rain") return "overcast-thunderstorms-with-rain";
  if (low == "thunderstorm-with-hail")          return "thunderstorm-with-hail";
  if (low == "thunderstorm-with-rain")          return "thunderstorm-with-rain";
  if (low == "partly-cloudy-and-light-rain")    return "partly-cloudy-and-light-rain";
  if (low == "partly-cloudy-and-rain")          return "partly-cloudy-and-rain";
  if (low == "overcast-and-light-rain")         return "overcast-and-light-rain";
  if (low == "overcast-and-rain")               return "overcast-and-rain";
  if (low == "cloudy-and-light-rain")           return "cloudy-and-light-rain";
  if (low == "cloudy-and-rain")                 return "cloudy-and-rain";
  if (low == "overcast-and-wet-snow")           return "overcast-and-wet-snow";
  if (low == "partly-cloudy-and-light-snow")    return "partly-cloudy-and-light-snow";
  if (low == "partly-cloudy-and-snow")          return "partly-cloudy-and-snow";
  if (low == "overcast-and-light-snow")         return "overcast-and-light-snow";
  if (low == "overcast-and-snow")               return "overcast-and-snow";
  if (low == "cloudy-and-light-snow")           return "cloudy-and-light-snow";
  if (low == "cloudy-and-snow")                 return "cloudy-and-snow";
  if (low == "clear" || low == "sunny") return "clear";
  if (low == "partly-cloudy" || low == "partlycloudy") return "partly-cloudy";
  if (low == "cloudy") return "cloudy";
  if (low == "overcast") return "overcast";
  if (low == "drizzle") return "drizzle";
  if (low == "light-rain") return "light-rain";
  if (low == "rain") return "rain";
  if (low == "moderate-rain") return "moderate-rain";
  if (low == "heavy-rain") return "heavy-rain";
  if (low == "continuous-heavy-rain") return "continuous-heavy-rain";
  if (low == "showers") return "showers";
  if (low == "wet-snow" || low == "sleet") return "wet-snow";
  if (low == "light-snow") return "light-snow";
  if (low == "snow") return "snow";
  if (low == "snow-showers" || low == "snowfall") return "snow-showers";
  if (low == "snowstorm") return "snowstorm";
  if (low == "heavy-snowstorm") return "heavy-snowstorm";
  if (low == "thunderstorm") return "thunderstorm";
  if (low == "hail") return "hail";
  if (low == "fog")  return "fog";
  if (low == "mist") return "mist";
  if (low == "haze") return "haze";
  if (low == "smoke" || low == "smog") return "smoke";
  if (low == "dust") return "dust";
  if (low == "dust-storm") return "dust-storm";
  if (low == "sand") return "sand";
  if (low == "ash" || low == "volcanic-eruption") return "ash";
  if (low == "storm") return "storm";

  if (low.indexOf("гроз") >= 0) {
    if (low.indexOf("град") >= 0) return "thunderstorm-with-hail";
    if (low.indexOf("дожд") >= 0 || low.indexOf("лив") >= 0) return "thunderstorm-with-rain";
    return "thunderstorm";
  }
  if (low.indexOf("град") >= 0) return "hail";
  if (low.indexOf("мокр") >= 0 && low.indexOf("снег") >= 0) return "wet-snow";
  if (low.indexOf("метел") >= 0 || low.indexOf("буран") >= 0) return "snowstorm";
  if (low.indexOf("лив") >= 0) return "showers";
  if (low.indexOf("морос") >= 0) return "drizzle";
  if (low.indexOf("дожд") >= 0) {
    if (low.indexOf("затяж") >= 0 || low.indexOf("продолж") >= 0) return "continuous-heavy-rain";
    if (low.indexOf("сильн") >= 0) return "heavy-rain";
    if (low.indexOf("умерен") >= 0) return "moderate-rain";
    if (low.indexOf("слаб") >= 0 || low.indexOf("небольш") >= 0) return "light-rain";
    return "rain";
  }
  if (low.indexOf("снег") >= 0 || low.indexOf("снеж") >= 0) {
    if (low.indexOf("сильн") >= 0 || low.indexOf("снегопад") >= 0) return "snow-showers";
    if (low.indexOf("слаб") >= 0 || low.indexOf("небольш") >= 0) return "light-snow";
    return "snow";
  }
  if (low.indexOf("туман") >= 0) return "fog";
  if (low.indexOf("дымк") >= 0) return "haze";
  if (low.indexOf("смог") >= 0 || low.indexOf("дым") >= 0) return "smoke";
  if (low.indexOf("пыльн") >= 0 || low.indexOf("пыл") >= 0) return "dust";
  if (low.indexOf("пес") >= 0) return "sand";
  if (low.indexOf("пеп") >= 0 || low.indexOf("вулкан") >= 0) return "ash";
  if (low.indexOf("шторм") >= 0) return "storm";
  if (low.indexOf("пасмур") >= 0) return "overcast";
  if (low.indexOf("малооблач") >= 0 || low.indexOf("переменн") >= 0 || low.indexOf("прояснен") >= 0) return "partly-cloudy";
  if (low.indexOf("облач") >= 0) return "cloudy";
  if (low.indexOf("ясн") >= 0) return "clear";

  if (low.indexOf("thunder") >= 0) {
    if (low.indexOf("hail") >= 0) return "thunderstorm-with-hail";
    if (low.indexOf("rain") >= 0) return "thunderstorm-with-rain";
    return "thunderstorm";
  }
  if (low.indexOf("hail") >= 0) return "hail";
  if (low.indexOf("sleet") >= 0 || (low.indexOf("wet") >= 0 && low.indexOf("snow") >= 0)) return "wet-snow";
  if (low.indexOf("snowstorm") >= 0 || low.indexOf("blizzard") >= 0) return "snowstorm";
  if (low.indexOf("showers") >= 0 && low.indexOf("snow") >= 0) return "snow-showers";
  if (low.indexOf("showers") >= 0 || low.indexOf("shower") >= 0) return "showers";
  if (low.indexOf("drizzle") >= 0) return "drizzle";
  if (low.indexOf("rain") >= 0) {
    if (low.indexOf("continuous") >= 0) return "continuous-heavy-rain";
    if (low.indexOf("heavy") >= 0) return "heavy-rain";
    if (low.indexOf("moderate") >= 0) return "moderate-rain";
    if (low.indexOf("light") >= 0) return "light-rain";
    return "rain";
  }
  if (low.indexOf("snow") >= 0) {
    if (low.indexOf("heavy") >= 0) return "snow-showers";
    if (low.indexOf("light") >= 0) return "light-snow";
    return "snow";
  }
  if (low.indexOf("fog") >= 0) return "fog";
  if (low.indexOf("mist") >= 0) return "mist";
  if (low.indexOf("haze") >= 0) return "haze";
  if (low.indexOf("smoke") >= 0 || low.indexOf("smog") >= 0) return "smoke";
  if (low.indexOf("dust-storm") >= 0) return "dust-storm";
  if (low.indexOf("dust") >= 0) return "dust";
  if (low.indexOf("sand") >= 0) return "sand";
  if (low.indexOf("ash") >= 0 || low.indexOf("volcanic") >= 0) return "ash";
  if (low.indexOf("storm") >= 0) return "storm";
  if (low.indexOf("overcast") >= 0) return "overcast";
  if (low.indexOf("partly-cloudy") >= 0) return "partly-cloudy";
  if (low.indexOf("cloudy") >= 0) return "cloudy";
  if (low.indexOf("clear") >= 0) return "clear";

  return low;
}

String yandexIconToCond(String icon) {
  icon.trim();
  if (!icon.length()) return "";

  String s = toLowerCopy(icon);

  if (s.endsWith("_d") || s.endsWith("_n") || s.endsWith("-d") || s.endsWith("-n")) {
    s.remove(s.length() - 2);
  }

  if (s == "skc") return "clear";
  if (s == "fg")  return "fog";
  if (s == "bkn") return "partly-cloudy";
  if (s == "ovc") return "overcast";
  if (s == "ovc-ts-ra" || s == "ovc_ts_ra") return "overcast-thunderstorms-with-rain";
  if (s == "ovc-ts-ha" || s == "ovc_ts_ha") return "thunderstorm-with-hail";
  if (s.indexOf("ts") >= 0 && s.indexOf("ha") >= 0) return "thunderstorm-with-hail";
  if (s.indexOf("ts") >= 0 && s.indexOf("ra") >= 0) return "thunderstorm-with-rain";
  if (s.indexOf("ts") >= 0) return "thunderstorm";
  if (s.indexOf("ra_sn") >= 0 || s.indexOf("ra-sn") >= 0) return "wet-snow";
  if (s.indexOf("+ra") >= 0) {
    if (s.indexOf("ovc") >= 0) return "overcast-and-rain";
    return "heavy-rain";
  }
  if (s.indexOf("-ra") >= 0) {
    if (s.indexOf("ovc") >= 0) return "overcast-and-light-rain";
    if (s.indexOf("bkn") >= 0) return "partly-cloudy-and-light-rain";
    return "light-rain";
  }
  if (s.indexOf("_ra") >= 0 || s.indexOf("-ra") >= 0 || s.endsWith("ra")) {
    if (s.indexOf("ovc") >= 0) return "cloudy-and-rain";
    if (s.indexOf("bkn") >= 0) return "partly-cloudy-and-rain";
    return "rain";
  }

  if (s.indexOf("+sn") >= 0) return "snow-showers";
  if (s.indexOf("-sn") >= 0) {
    if (s.indexOf("ovc") >= 0) return "overcast-and-light-snow";
    if (s.indexOf("bkn") >= 0) return "partly-cloudy-and-light-snow";
    return "light-snow";
  }
  if (s.indexOf("_sn") >= 0 || s.indexOf("-sn") >= 0 || s.endsWith("sn")) {
    if (s.indexOf("ovc") >= 0) return "cloudy-and-snow";
    if (s.indexOf("bkn") >= 0) return "partly-cloudy-and-snow";
    return "snow";
  }

  if (s == "-bl") return "snowstorm";
  if (s == "bl")  return "heavy-snowstorm";
  if (s == "dst") return "dust";
  if (s == "du_st" || s == "du-st") return "dust-storm";
  if (s == "smog") return "smoke";
  if (s == "strm") return "storm";
  if (s == "vlka") return "ash";
  if (s.indexOf("ha") >= 0) return "hail";

  return "";
}

String buildYandexRuFromCond(const String& cond, float temp) {
  String desc;

  if      (cond == "clear")                              desc = "ясно";
  else if (cond == "partly-cloudy")                      desc = "малооблачно";
  else if (cond == "cloudy")                             desc = "облачно";
  else if (cond == "overcast")                           desc = "пасмурно";
  else if (cond == "drizzle")                            desc = "морось";
  else if (cond == "light-rain")                         desc = "слабый дождь";
  else if (cond == "rain")                               desc = "идёт дождь";
  else if (cond == "moderate-rain")                      desc = "умеренный дождь";
  else if (cond == "heavy-rain")                         desc = "сильный дождь";
  else if (cond == "continuous-heavy-rain")              desc = "затяжной сильный дождь";
  else if (cond == "showers")                            desc = "ливень";
  else if (cond == "wet-snow")                           desc = "мокрый снег";
  else if (cond == "light-snow")                         desc = "небольшой снег";
  else if (cond == "snow")                               desc = "идёт снег";
  else if (cond == "snow-showers")                       desc = "снегопад";
  else if (cond == "hail")                               desc = "град";
  else if (cond == "thunderstorm")                       desc = "гроза";
  else if (cond == "thunderstorm-with-rain")             desc = "дождь, гроза";
  else if (cond == "thunderstorm-with-hail")             desc = "гроза, град";
  else if (cond == "partly-cloudy-and-light-rain")       desc = "малооблачно, слабый дождь";
  else if (cond == "partly-cloudy-and-rain")             desc = "малооблачно, дождь";
  else if (cond == "overcast-and-light-rain")            desc = "пасмурно, слабый дождь";
  else if (cond == "overcast-and-rain")                  desc = "пасмурно, сильный дождь";
  else if (cond == "overcast-thunderstorms-with-rain")   desc = "пасмурно, сильный дождь, гроза";
  else if (cond == "cloudy-and-light-rain")              desc = "облачно, слабый дождь";
  else if (cond == "cloudy-and-rain")                    desc = "облачно, дождь";
  else if (cond == "overcast-and-wet-snow")              desc = "пасмурно, мокрый снег";
  else if (cond == "partly-cloudy-and-light-snow")       desc = "малооблачно, небольшой снег";
  else if (cond == "partly-cloudy-and-snow")             desc = "малооблачно, снег";
  else if (cond == "overcast-and-light-snow")            desc = "пасмурно, небольшой снег";
  else if (cond == "overcast-and-snow")                  desc = "пасмурно, снегопад";
  else if (cond == "cloudy-and-light-snow")              desc = "облачно, небольшой снег";
  else if (cond == "cloudy-and-snow")                    desc = "облачно, снег";
  else if (cond == "fog")                                desc = "туман";
  else if (cond == "mist" || cond == "haze")             desc = "дымка";
  else if (cond == "smoke")                              desc = "смог";
  else if (cond == "dust")                               desc = "пыльная мгла";
  else if (cond == "dust-storm")                         desc = "пыльная буря";
  else if (cond == "sand")                               desc = "песчаная мгла";
  else if (cond == "ash")                                desc = "вулканический пепел";
  else if (cond == "storm")                              desc = "шторм";
  else if (cond == "snowstorm")                          desc = "метель";
  else if (cond == "heavy-snowstorm")                    desc = "сильная метель";

  if (!desc.length()) desc = tempToRussianFeeling(temp);
  return desc;
}

bool httpsGetToString(const char* host, const String& path, uint32_t timeoutMs, String& outPayload) {
  WiFiClientSecure client;
  client.setInsecure();

  if (!client.connect(host, 443)) return false;
  client.print(String("GET ") + path + " HTTP/1.1\r\n"
               "Host: " + host + "\r\n"
               "User-Agent: FieryLedLamp\r\n"
               "Accept: application/json\r\n"
               "Accept-Encoding: identity\r\n"
               "Connection: close\r\n\r\n");

  bool headersEnded = false;
  uint32_t tmr = millis();
  outPayload = "";

  while (millis() - tmr < timeoutMs) {
    while (client.available()) {
      String line = client.readStringUntil('\n');
      if (!headersEnded) {
        if (line == "\r") headersEnded = true;
      } else {
        outPayload += line;
      }
      tmr = millis();
    }
    if (!client.connected() && !client.available()) break;
    delay(1);
  }

  client.stop();
  return outPayload.length() > 0;
}

String urlEncodeUtf8(const String& value) {
  static const char hex[] = "0123456789ABCDEF";
  String encoded;
  encoded.reserve(value.length() * 3);

  for (size_t i = 0; i < value.length(); i++) {
    uint8_t c = (uint8_t)value[i];
    bool unreserved = (c >= 'A' && c <= 'Z') ||
                      (c >= 'a' && c <= 'z') ||
                      (c >= '0' && c <= '9') ||
                      c == '-' || c == '_' || c == '.' || c == '~';

    if (unreserved) {
      encoded += (char)c;
    } else {
      encoded += '%';
      encoded += hex[(c >> 4) & 0x0F];
      encoded += hex[c & 0x0F];
    }
  }

  return encoded;
}

String getYandexSuggestText(JsonVariantConst value) {
  if (value.is<const char*>()) {
    String text = value.as<const char*>();
    text.trim();
    return text;
  }

  if (value.is<JsonObjectConst>()) {
    String text = value["text"] | "";
    if (!text.length()) text = value["value"] | "";
    if (!text.length()) text = value["title"] | "";
    text.trim();
    return text;
  }

  return F("");
}

String getYandexSuggestGeoId(JsonVariantConst result) {
  String geoId = result["geoid"].as<String>();
  if (!geoId.length()) geoId = result["geo_id"].as<String>();
  if (!geoId.length()) geoId = result["data"]["geoid"].as<String>();
  if (!geoId.length()) geoId = result["data"]["geo_id"].as<String>();
  geoId.trim();
  return isValidYandexGeoId(geoId) ? geoId : String();
}

String getYandexSuggestTitle(JsonVariantConst result) {
  String title = getYandexSuggestText(result["title"]);
  if (!title.length()) title = getYandexSuggestText(result["text"]);
  if (!title.length()) title = getYandexSuggestText(result["name"]);
  if (!title.length()) title = getYandexSuggestText(result["data"]["title"]);
  if (!title.length()) title = getYandexSuggestText(result["data"]["text"]);
  title.trim();
  return title;
}

String weatherCityNameForMatch(String cityName) {
  int separator = cityName.indexOf(',');
  if (separator > 0) cityName = cityName.substring(0, separator);
  cityName.trim();
  return normalizeWeatherCityRu(cityName);
}

bool getYandexGeoIdByCityName(String cityQuery, String& geoId, String& cityTitle) {
  geoId = F("");
  cityTitle = F("");
  cityQuery.trim();

  if (!cityQuery.length() || WiFi.status() != WL_CONNECTED) return false;

  String path = F("/suggest-geo?search_type=tune&v=9&results=5&lang=ru_RU&callback=callback&part=");
  path += urlEncodeUtf8(cityQuery);

  String payload;
  if (!httpsGetToString("suggest-maps.yandex.ru", path, 8000, payload)) return false;

  // Сервис может вернуть JSON-объект, JSON-массив или JSONP: callback({...}).
  int objectStart = payload.indexOf('{');
  int arrayStart = payload.indexOf('[');
  int jsonStart;
  char jsonOpen;
  if (objectStart >= 0 && (arrayStart < 0 || objectStart < arrayStart)) {
    jsonStart = objectStart;
    jsonOpen = '{';
  } else {
    jsonStart = arrayStart;
    jsonOpen = '[';
  }
  int jsonEnd = (jsonOpen == '{') ? payload.lastIndexOf('}') : payload.lastIndexOf(']');
  if (jsonStart < 0 || jsonEnd <= jsonStart) return false;

  DynamicJsonDocument doc(24576);
  DeserializationError err = deserializeJson(doc, payload.substring(jsonStart, jsonEnd + 1));
  if (err) return false;

  JsonArrayConst results;
  if (doc.is<JsonArray>()) results = doc.as<JsonArrayConst>();
  if (results.isNull()) results = doc["results"].as<JsonArrayConst>();
  if (results.isNull()) results = doc["items"].as<JsonArrayConst>();
  if (results.isNull()) results = doc["suggestions"].as<JsonArrayConst>();
  if (results.isNull()) results = doc["data"]["results"].as<JsonArrayConst>();
  if (results.isNull()) results = doc["data"]["items"].as<JsonArrayConst>();
  if (results.isNull() || results.size() == 0) return false;

  String normalizedQuery = weatherCityNameForMatch(cityQuery);
  String firstGeoId;
  String firstTitle;

  for (JsonVariantConst result : results) {
    String candidateGeoId = getYandexSuggestGeoId(result);
    if (!candidateGeoId.length()) continue;

    String candidateTitle = getYandexSuggestTitle(result);
    if (!firstGeoId.length()) {
      firstGeoId = candidateGeoId;
      firstTitle = candidateTitle;
    }

    if (candidateTitle.length() &&
        weatherCityNameForMatch(candidateTitle) == normalizedQuery) {
      geoId = candidateGeoId;
      cityTitle = candidateTitle;
      break;
    }
  }

  if (!geoId.length()) {
    geoId = firstGeoId;
    cityTitle = firstTitle;
  }

  if (!isValidYandexGeoId(geoId)) {
    geoId = F("");
    cityTitle = F("");
    return false;
  }

  int separator = cityTitle.indexOf(',');
  if (separator > 0) cityTitle = cityTitle.substring(0, separator);
  cityTitle.trim();
  if (!cityTitle.length()) {
    cityTitle = cityQuery;
    separator = cityTitle.indexOf(',');
    if (separator > 0) cityTitle = cityTitle.substring(0, separator);
    cityTitle.trim();
  }

  return cityTitle.length() > 0;
}

bool containsRussianUtf8(const String& value) {
  for (size_t i = 0; i < value.length(); i++) {
    uint8_t c = (uint8_t)value[i];
    if (c == 0xD0 || c == 0xD1) return true;
  }
  return false;
}

bool getOpenWeatherLatinName(String cityRu, String& cityLatin) {
  cityLatin = F("");
  cityRu.trim();

  if (!cityRu.length() || WiFi.status() != WL_CONNECTED || weatherApiKey.length() <= 10) {
    return false;
  }

  String path = F("/geo/1.0/direct?q=");
  path += urlEncodeUtf8(cityRu);
  path += F("&limit=1&appid=");
  path += weatherApiKey;

  String payload;
  if (!httpsGetToString("api.openweathermap.org", path, 8000, payload)) return false;

  DynamicJsonDocument doc(12288);
  DeserializationError err = deserializeJson(doc, payload);
  if (err || !doc.is<JsonArray>() || doc.size() == 0) return false;

  cityLatin = doc[0]["local_names"]["en"] | "";
  cityLatin.trim();

  if (!cityLatin.length()) {
    cityLatin = doc[0]["name"] | "";
    cityLatin.trim();
  }

  if (!cityLatin.length() || containsRussianUtf8(cityLatin)) {
    cityLatin = F("");
    return false;
  }

  return true;
}

bool upgradeYandexOnlyCityForOpenWeather() {
  if (weatherCity.length()) return true;
  if (!yandexGeoId.length() || yandexGeoId == F("0")) return false;

  String yandexOnlyValue = yandexGeoId + F("|");
  String citiesJson = readFile(F("weather_city.json"), 16384);
  if (citiesJson == F("Failed") || citiesJson == F("Large") || !citiesJson.length()) return false;

  String cityRu;
  {
    DynamicJsonDocument citiesDoc(16384);
    DeserializationError citiesError = deserializeJson(citiesDoc, citiesJson);
    if (citiesError || !citiesDoc.is<JsonObject>()) return false;

    cityRu = citiesDoc[yandexOnlyValue] | "";
    cityRu.trim();
  }

  if (!cityRu.length()) {
    LOG.println(F("Погода: не найдено русское название текущего города"));
    return false;
  }

  String openWeatherCity;
  if (!findOpenWeatherCityByRussianName(cityRu, openWeatherCity) &&
      !getOpenWeatherLatinName(cityRu, openWeatherCity)) {
    LOG.println(F("Погода: не удалось определить название города для OpenWeather"));
    return false;
  }

  String weatherValue = yandexGeoId + F("|") + openWeatherCity;
  {
    DynamicJsonDocument citiesDoc(16384);
    DeserializationError citiesError = deserializeJson(citiesDoc, citiesJson);
    if (citiesError || !citiesDoc.is<JsonObject>()) return false;

    citiesDoc[weatherValue] = cityRu;
    if (weatherValue != yandexOnlyValue) citiesDoc.remove(yandexOnlyValue);
    if (citiesDoc.overflowed()) return false;

    String updatedCitiesJson;
    serializeJsonPretty(citiesDoc, updatedCitiesJson);
    updatedCitiesJson += '\n';
    String writeResult = writeFile(F("weather_city.json"), updatedCitiesJson);
    if (writeResult.startsWith(F("Failed"))) return false;
  }

  jsonWrite(configSetup, "weather_city", weatherValue);
  applyWeatherCityValue(weatherValue);
  saveConfig();

  LOG.printf("[WEATHER] Город обновлён для OpenWeather: %s -> %s\n",
             cityRu.c_str(), openWeatherCity.c_str());
  return true;
}

void updateWeather() {
  if (WiFi.status() != WL_CONNECTED) return;

  if (!preferYandex && weatherApiKey.length() > 10 && !weatherCity.length()) {
    upgradeYandexOnlyCityForOpenWeather();
  }

  bool success = false;
  actualYandex = false;

  if (preferYandex) {
    if (yandexGeoId.length() > 0 && yandexGeoId != "0") {
      success = getWeatherFromYandex();
      if (success) actualYandex = true;
    }

    if (!success && weatherApiKey.length() > 10 && weatherCity.length() > 0) {
      LOG.println(F("Погода: Яндекс не ответил, пробую OpenWeather"));
      success = getWeatherFromOpenWeather();
      if (success) actualYandex = false;
    }
  } else {
    if (weatherApiKey.length() > 10 && weatherCity.length() > 0) {
      success = getWeatherFromOpenWeather();
      if (success) actualYandex = false;
    }

    if (!success && yandexGeoId.length() > 0 && yandexGeoId != "0") {
      LOG.println(F("Погода: OpenWeather не ответил, пробую Яндекс.Погоду"));
      success = getWeatherFromYandex();
      if (success) actualYandex = true;
    }
  }

  if (!success) {
    currentTemp = -999.0f;
    currentCondition = "";
    LOG.println(F("Погода: нет данных"));
    return;
  }

  String weatherLine = F("На улице в городе ");
  weatherLine += getWeatherCityTitle();
  weatherLine += F(": ");
  if (currentTemp > 0) weatherLine += F("+");
  weatherLine += String(currentTemp, 1);
  weatherLine += F("°C");
  if (currentCondition.length() > 0) {
    weatherLine += F(", ");
    weatherLine += currentCondition;
  }
  LOG.println(weatherLine);
}
// -------------------- Yandex --------------------
bool getWeatherFromYandex() {
  String path = "/time/sync.json?geo=" + yandexGeoId + "&lang=ru";

  String payload;
  if (!httpsGetToString("yandex.com", path, 8000, payload)) return false;

  DynamicJsonDocument doc(4096);

  DeserializationError err = deserializeJson(doc, payload);
  if (err) return false;

  JsonObject clock = doc["clocks"][yandexGeoId];
  if (!clock.containsKey("weather")) return false;

  JsonObject w = clock["weather"];
  if (!w.containsKey("temp")) return false;

  currentTemp = w["temp"].as<float>();

  String cond = w["condition"] | "";
  if (!cond.length()) cond = w["weather"] | "";
  if (!cond.length()) cond = w["title"]  | "";
  if (!cond.length()) cond = w["name"]   | "";
  if (!cond.length()) {
    String icon = w["icon"] | "";
    cond = yandexIconToCond(icon);
  }

  cond = normalizeCond(cond);
  currentCondition = buildYandexRuFromCond(cond, currentTemp);
  return true;
}
// -------------------- OpenWeather --------------------
bool getWeatherFromOpenWeather() {
  String openWeatherCity = weatherCity;
  openWeatherCity.replace(" ", "%20");

  String path = "/data/2.5/weather?q=" + openWeatherCity +
                "&appid=" + weatherApiKey +
                "&units=metric&lang=ru";

  String payload;
  if (!httpsGetToString("api.openweathermap.org", path, 8000, payload)) return false;

  DynamicJsonDocument doc(2048);

DeserializationError err = deserializeJson(doc, payload);
  if (err) return false;
  if (!doc["main"]["temp"]) return false;

  currentTemp = doc["main"]["temp"].as<float>();
  currentCondition = doc["weather"][0]["description"] | "";
  if (!currentCondition.length()) currentCondition = tempToRussianFeeling(currentTemp);

return true;
}
#endif // USE_WEATHER == 1
