#include "Timer.h"
#include "arduino-timer.h"

namespace
{
  auto baseTimer = timer_create_default();
}

namespace psyduck
{
  namespace base
  {
    Timers::Task Timers::in(unsigned long delay, handler_t h, void *arg)
    {
      return baseTimer.in(delay, h, arg);
    }

    Timers::Task Timers::at(unsigned long time, handler_t h, void *arg)
    {
      return baseTimer.at(time, h, arg);
    }

    Timers::Task Timers::every(unsigned long interval, handler_t h, void *arg)
    {
      return baseTimer.every(interval, h, arg);
    }

    void Timers::cancel(Task &task)
    {
      baseTimer.cancel(task);
    }

    void Timers::tick()
    {
      baseTimer.tick();
    }

    int Timers::size()
    {
      return baseTimer.size();
    }
  };
}
