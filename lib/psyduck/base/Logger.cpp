#include "Logger.h"
#include <WString.h>
#include <Arduino.h>
#include <stdio.h>

namespace psyduck
{
  namespace base
  {
    ssize_t serialputc(void *p, const char *buf, size_t n)
    {
      Serial.write(buf, n);
      return n;
    }

    Logger::Logger(const char *source)
    {
      this->source = const_cast<char*>(source);
    }

    Logger::Logger(const char *sourcePrefix, char* sourcePostfix) 
    {
      this->source = new char[strlen(sourcePrefix) + strlen(sourcePostfix) + 1];
      strcpy(this->source, sourcePrefix);
      strcat(this->source, sourcePostfix);
    }

    Logger::Logger(const char *sourcePrefix, const char* sourcePostfix) {
      this->source = new char[strlen(sourcePrefix) + strlen(sourcePostfix) + 1];
      strcpy(this->source, sourcePrefix);
      strcat(this->source, sourcePostfix);
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

    FILE* open(void* ref) {
      cookie_io_functions_t fncs;
      fncs.write = serialputc;
      fncs.close = nullptr;
      return fopencookie(ref, "w", fncs);
    }

    void Logger::info(const char *format, ...)
    {
      this->writePrefix("INFO");
      va_list ap;
      va_start(ap, format);
      FILE *out = open(this);
      vfprintf(out, format, ap);
      fclose(out);
      va_end(ap);
      this->writePostfix();
    }

    void Logger::info(const __FlashStringHelper *fstring, ...) {
      this->writePrefix("INFO");
      size_t fmtLen = strlen_P((PGM_P) fstring);
      char format[fmtLen + 1];
      strcpy_P(format, (PGM_P) fstring);
      va_list ap;
      va_start(ap, format);
      FILE *out = open(this);
      vfprintf(out, format, ap);
      fclose(out);
      va_end(ap);
      this->writePostfix();
    }

    void Logger::warn(const char *format, ...)
    {
      this->writePrefix("WARN");
      va_list ap;
      va_start(ap, format);
      FILE *out = open(this);
      vfprintf(out, format, ap);
      fclose(out);
      va_end(ap);      
      this->writePostfix();
    }

    void Logger::warn(const __FlashStringHelper *fstring, ...) {
      this->writePrefix("WARN");
      size_t fmtLen = strlen_P((PGM_P) fstring);
      char format[fmtLen + 1];
      strcpy_P(format, (PGM_P) fstring);
      va_list ap;
      va_start(ap, format);
      FILE *out = open(this);
      vfprintf(out, format, ap);
      fclose(out);
      va_end(ap);
      this->writePostfix();
    }

    void Logger::error(const char *format, ...)
    {
      this->writePrefix("ERROR");
      va_list ap;
      va_start(ap, format);
      FILE *out = open(this);
      vfprintf(out, format, ap);
      fclose(out);
      va_end(ap);      
      this->writePostfix();
    }

    void Logger::error(const __FlashStringHelper *fstring, ...) {
      this->writePrefix("ERROR");
      size_t fmtLen = strlen_P((PGM_P) fstring);
      char format[fmtLen + 1];
      strcpy_P(format, (PGM_P) fstring);
      va_list ap;
      va_start(ap, format);
      FILE *out = open(this);
      vfprintf(out, format, ap);
      fclose(out);
      va_end(ap);
      this->writePostfix();
    }

    void Logger::debug(const char *format, ...)
    {
      this->writePrefix("DEBUG");
      va_list ap;
      va_start(ap, format);
      FILE *out = open(this);
      vfprintf(out, format, ap);
      fclose(out);
      va_end(ap);      
      this->writePostfix();
    }

    void Logger::debug(const __FlashStringHelper *fstring, ...) {
      this->writePrefix("DEBUG");
      size_t fmtLen = strlen_P((PGM_P) fstring);
      char format[fmtLen + 1];
      strcpy_P(format, (PGM_P) fstring);
      va_list ap;
      va_start(ap, format);
      FILE *out = open(this);
      vfprintf(out, format, ap);
      fclose(out);
      va_end(ap);
      this->writePostfix();
    }
  }
}