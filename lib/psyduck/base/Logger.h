#ifndef _CHRISPYDUCK_LOGGING_H
#define _CHRISPYDUCK_LOGGING_H

#ifndef LOGGING_DEVICE
#define LOGGING_DEVICE Serial
#endif

#include <Arduino.h>

namespace psyduck
{
  namespace base
  {
    enum LogLevel
    {
      LOG_LEVEL_ERROR,
      LOG_LEVEL_WARN,
      LOG_LEVEL_INFO,
      LOG_LEVEL_DEBUG,
      LOG_LEVEL_TRACE
    };

    class Logger
    {
    public:
      Logger(const char *source);
      Logger(const char *sourcePrefix, char *sourcePostfix);
      Logger(const char *sourcePrefix, const char *sourcePostfix);

      void info(const char *format, ...);
      void info(const __FlashStringHelper *format, ...);
      void warn(const char *format, ...);
      void warn(const __FlashStringHelper *format, ...);
      void error(const char *format, ...);
      void error(const __FlashStringHelper *format, ...);
      void debug(const char *format, ...);
      void debug(const __FlashStringHelper *format, ...);

      void setLogLevel(LogLevel logLevel);

    private:
      char *source = nullptr;

      LogLevel logLevel = LogLevel::LOG_LEVEL_INFO;

      void writePrefix(LogLevel level);
      void writePostfix();
    };
  }
}

#endif