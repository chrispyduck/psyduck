#include "Logger.h"
#include <WString.h>
#include <Arduino.h>
#include <stdio.h>

#pragma GCC diagnostic ignored "-Wvarargs"

#define LOG_FN_CHAR(level, format) \
  if (this->logLevel < level)      \
  {                                \
    return;                        \
  }                                \
  this->writePrefix(level);        \
  va_list ap;                      \
  va_start(ap, format);            \
  FILE *out = open(this);          \
  vfprintf(out, format, ap);       \
  fclose(out);                     \
  va_end(ap);                      \
  this->writePostfix();

#define LOG_FN_FLASHSTRING(level, fstring)  \
  if (this->logLevel < level)               \
  {                                         \
    return;                                 \
  }                                         \
  size_t fmtLen = strlen_P((PGM_P)fstring); \
  char format[fmtLen + 1];                  \
  strcpy_P(format, (PGM_P)fstring);         \
  LOG_FN_CHAR(level, format);

#define LOG_FNS_FOR_LEVEL(level, functionName)                       \
  void Logger::functionName(const char *format, ...)                 \
  {                                                                  \
    LOG_FN_CHAR(level, format);                                      \
  }                                                                  \
                                                                     \
  void Logger::functionName(const __FlashStringHelper *fstring, ...) \
  {                                                                  \
    LOG_FN_FLASHSTRING(level, fstring);                              \
  }

namespace psyduck
{
  namespace base
  {

    const char *logLevelNames[] = {
        "ERROR",
        "WARN ",
        "INFO ",
        "DEBUG",
        "TRACE"};

    ssize_t serialputc(void *p, const char *buf, size_t n)
    {
      Serial.write(buf, n);
      return n;
    }

    Logger::Logger(const char *source)
    {
      this->source = const_cast<char *>(source);
    }

    Logger::Logger(const char *sourcePrefix, char *sourcePostfix)
    {
      this->source = new char[strlen(sourcePrefix) + strlen(sourcePostfix) + 1];
      strcpy(this->source, sourcePrefix);
      strcat(this->source, sourcePostfix);
    }

    Logger::Logger(const char *sourcePrefix, const char *sourcePostfix)
    {
      this->source = new char[strlen(sourcePrefix) + strlen(sourcePostfix) + 1];
      strcpy(this->source, sourcePrefix);
      strcat(this->source, sourcePostfix);
    }

    void Logger::writePrefix(LogLevel level)
    {
      LOGGING_DEVICE.write(logLevelNames[level]);
      LOGGING_DEVICE.write(' ');
      LOGGING_DEVICE.write((char)(xPortGetCoreID() + 48));
      LOGGING_DEVICE.write(" [");
      LOGGING_DEVICE.write(this->source);
      LOGGING_DEVICE.write("] ");
    }

    void Logger::writePostfix()
    {
      LOGGING_DEVICE.write("\n");
    }

    FILE *open(void *ref)
    {
      cookie_io_functions_t fncs;
      fncs.write = serialputc;
      fncs.close = nullptr;
      return fopencookie(ref, "w", fncs);
    }

    LOG_FNS_FOR_LEVEL(LogLevel::LOG_LEVEL_ERROR, error);
    LOG_FNS_FOR_LEVEL(LogLevel::LOG_LEVEL_WARN, warn);
    LOG_FNS_FOR_LEVEL(LogLevel::LOG_LEVEL_INFO, info);
    LOG_FNS_FOR_LEVEL(LogLevel::LOG_LEVEL_DEBUG, debug);

    void Logger::setLogLevel(LogLevel logLevel)
    {
      if (logLevel < LogLevel::LOG_LEVEL_ERROR)
        logLevel = LogLevel::LOG_LEVEL_ERROR;
      else if (logLevel > LogLevel::LOG_LEVEL_TRACE)
        logLevel = LogLevel::LOG_LEVEL_TRACE;

      this->logLevel = logLevel;
    }
  }
}