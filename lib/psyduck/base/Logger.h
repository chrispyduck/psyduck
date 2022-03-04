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
    class Logger
    {
    public:
      Logger(const char *source);
      Logger(const char *sourcePrefix, char* sourcePostfix);
      Logger(const char *sourcePrefix, const char* sourcePostfix);

      void info(const char *format, ...);
      void info(const __FlashStringHelper *format, ...);
      void warn(const char *format, ...);
      void warn(const __FlashStringHelper *format, ...);
      void error(const char *format, ...);
      void error(const __FlashStringHelper *format, ...);
      void debug(const char *format, ...);
      void debug(const __FlashStringHelper *format, ...);

    private:
      char *source = nullptr;

      void writePrefix(const char* level);
      void writePostfix();
    };
  }
}

#endif