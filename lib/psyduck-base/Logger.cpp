#include "Logger.h"
#include <WString.h>
#include <Arduino.h>
#include <stdio.h>

namespace
{
  FILE stdiostr;

  extern "C"
  {
    int serialputc(char c, __attribute__((unused)) FILE *fp)
    {
      Serial.write(c);
      return 0;
    }
  }
}

namespace psyduck
{
  namespace base
  {

    void setup()
    {
      //fdevopen(&serialputc, 0);
      //fdev_setup_stream(&stdiostr, serialputc, NULL, _FDEV_SETUP_WRITE);
    }

    Logger::Logger(const char *source)
    {
      this->source = source;
    }

    void Logger::writePrefix(const char *level)
    {
      LOGGING_DEVICE.write(level);
      LOGGING_DEVICE.write(" [");
      LOGGING_DEVICE.write(this->source);
      LOGGING_DEVICE.write("] ");
    }

    void Logger::writePostfix()
    {
      LOGGING_DEVICE.write("\n");
    }

    void Logger::info(const char *format, ...)
    {
      this->writePrefix("INFO");
      va_list ap;
      va_start(ap, format);
      vfprintf(&stdiostr, format, ap);
      va_end(ap);
      this->writePostfix();
    }

    void Logger::warn(const char *format, ...)
    {
      this->writePrefix("WARN");
      va_list ap;
      va_start(ap, format);
      vfprintf(&stdiostr, format, ap);
      va_end(ap);
      this->writePostfix();
    }

    void Logger::error(const char *format, ...)
    {
      this->writePrefix("ERROR");
      va_list ap;
      va_start(ap, format);
      vfprintf(&stdiostr, format, ap);
      va_end(ap);
      this->writePostfix();
    }

    void Logger::debug(const char *format, ...)
    {
      this->writePrefix("DEBUG");
      va_list ap;
      va_start(ap, format);
      vfprintf(&stdiostr, format, ap);
      va_end(ap);
      this->writePostfix();
    }
  }
}