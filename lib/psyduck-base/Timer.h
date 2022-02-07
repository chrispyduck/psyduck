#ifndef _CHRISPYDUCK_TIMER_H
#define _CHRISPYDUCK_TIMER_H

#ifndef ARDUINO
#define ARDUINO 100
#endif

#include <arduino-timer.h>

namespace
{
  auto baseTimer = timer_create_default();
}

namespace psyduck
{
  namespace base
  {
    class Timers
    {
    public:
      typedef uintptr_t Task;
      typedef bool (*handler_t)(void *opaque);

      static Task in(unsigned long delay, handler_t h, void *arg = 0)
      {
        return baseTimer.in(delay, h, arg);
      }

      static Task at(unsigned long time, handler_t h, void *arg = 0)
      {
        return baseTimer.at(time, h, arg);
      }

      static Task every(unsigned long interval, handler_t h, void *arg = 0)
      {
        return baseTimer.every(interval, h, arg);
      }

      static void cancel(Task &task)
      {
        baseTimer.cancel(task);
      }

      static void tick()
      {
        baseTimer.tick();
      }
    };
  }
}
#endif
