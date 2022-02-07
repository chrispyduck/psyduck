#ifndef _CHRISPYDUCK_LOGGING_H
#define _CHRISPYDUCK_LOGGING_H

#ifndef LOGGING_DEVICE
#define LOGGING_DEVICE Serial
#endif

namespace psyduck
{
  namespace base
  {
    class Logger
    {
    public:
      Logger(const char *source);

      void info(const char *format, ...);
      void warn(const char *format, ...);
      void error(const char *format, ...);
      void debug(const char *format, ...);

    private:
      const char *source;

      void writePrefix(const char* level);
      void writePostfix();
    };
  }
}

#endif