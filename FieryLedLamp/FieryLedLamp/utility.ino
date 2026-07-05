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


// восстановление настроек эффектов на настройки по умолчанию
void restoreSettings()
{
    for (uint8_t i = 0; i < MODE_AMOUNT; i++) {
      modes[i].Brightness = pgm_read_byte(&defaultSettings[i][0]);
      modes[i].Speed      = pgm_read_byte(&defaultSettings[i][1]);
      modes[i].Scale      = pgm_read_byte(&defaultSettings[i][2]);
    }
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
