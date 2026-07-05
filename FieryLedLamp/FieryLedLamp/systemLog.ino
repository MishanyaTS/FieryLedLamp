#include "SystemLog.h"

SystemLog& SystemLog::instance() {
  static SystemLog inst;
  return inst;
}

void SystemLog::setEnabled(bool enabled) {
  _enabled = enabled;
}

bool SystemLog::isEnabled() const {
  return _enabled;
}

void SystemLog::clear() {
  _buffer = "";
  _lineStart = true;
}

String SystemLog::getAll() {
  return _buffer;
}

void SystemLog::addPrefixIfNeeded() {
  if (!_lineStart) return;
  char prefix[24];
  snprintf(prefix, sizeof(prefix), "[%10lu] ", (unsigned long)millis());
  _buffer += prefix;
  _lineStart = false;
}

void SystemLog::appendToBuffer(char c) {
  if (!_enabled) return;
  addPrefixIfNeeded();
  _buffer += c;
  if (c == '\n') _lineStart = true;

  if (_buffer.length() > MAX_BUFFER) {
    int cut = _buffer.length() - MAX_BUFFER;
    int nl = _buffer.indexOf('\n', cut);
    if (nl >= 0 && nl + 1 < (int)_buffer.length()) cut = nl + 1;
    _buffer.remove(0, cut);
  }
}

void SystemLog::appendString(const char *s) {
  if (!s) return;
  while (*s) appendToBuffer(*s++);
}

size_t SystemLog::write(uint8_t c) {
  Serial.write(c);
  appendToBuffer((char)c);
  return 1;
}

size_t SystemLog::write(const uint8_t *buffer, size_t size) {
  Serial.write(buffer, size);
  if (_enabled && buffer) {
    for (size_t i = 0; i < size; i++) appendToBuffer((char)buffer[i]);
  }
  return size;
}

void SystemLog::appendFormatted(const char *format, va_list args, bool progmem) {
  if (!format) return;
  char stackBuf[256];
  va_list copy;
  va_copy(copy, args);
#ifdef ESP32
  int len = progmem ? vsnprintf_P(stackBuf, sizeof(stackBuf), format, copy)
                    : vsnprintf(stackBuf, sizeof(stackBuf), format, copy);
#else
  int len = vsnprintf(stackBuf, sizeof(stackBuf), format, copy);
#endif
  va_end(copy);

  if (len < 0) return;
  if (len < (int)sizeof(stackBuf)) {
    print(stackBuf);
    return;
  }

  char *heapBuf = (char*)malloc((size_t)len + 1);
  if (!heapBuf) {
    print(stackBuf);
    return;
  }
#ifdef ESP32
  if (progmem) vsnprintf_P(heapBuf, (size_t)len + 1, format, args);
  else         vsnprintf(heapBuf, (size_t)len + 1, format, args);
#else
  vsnprintf(heapBuf, (size_t)len + 1, format, args);
#endif
  print(heapBuf);
  free(heapBuf);
}

size_t SystemLog::printf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  appendFormatted(format, args, false);
  va_end(args);
  return 0;
}

size_t SystemLog::printf_P(PGM_P format, ...) {
  va_list args;
  va_start(args, format);
  appendFormatted((const char*)format, args, true);
  va_end(args);
  return 0;
}

void SystemLog::add(const char *format, ...) {
  va_list args;
  va_start(args, format);
  appendFormatted(format, args, false);
  va_end(args);
  println();
}

void SystemLog::add(const String &text) {
  println(text);
}

void SystemLog::add(char c) {
  write((uint8_t)c);
}
