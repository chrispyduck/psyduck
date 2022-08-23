#ifndef _CHRISPYDUCK_TIMER_H
#define _CHRISPYDUCK_TIMER_H

#ifndef ARDUINO
#define ARDUINO 100
#endif

#include "arduino-timer.h"
//#include <ESP32_New_TimerInterrupt.h>

namespace psyduck
{
  namespace base
  {
    class Timers
    {

    public:
      typedef uintptr_t Task;
      typedef bool (*handler_t)(void *opaque);
      static Task in(unsigned long delay, handler_t h, void *arg = 0);
      static Task at(unsigned long time, handler_t h, void *arg = 0);
      static Task every(unsigned long interval, handler_t h, void *arg = 0);
      static void cancel(Task &task);
      static void tick();
      static int size();
    };
  }
}
#endif
