#ifndef _PSYDUCK_GPIO
#define _PSYDUCK_GPIO

#include <Arduino.h>

#define MOMENTARY_SWITCH_DELAY 80

namespace psyduck
{
  namespace gpio
  {
    struct GpioNonLatchingSwitch
    {
      short pinOn;
    };
    struct GpioLatchingSwitch
    {
      short pinOn;
      short pinOff;
    };

    struct GpioState {
      uint8_t ledcCounter = 0;
    };

    class GPIO
    {
    public:
      static void setSwitch(GpioLatchingSwitch& config, bool desiredValue);
      static void setSwitch(GpioNonLatchingSwitch& config, bool desiredValue);
      static uint8_t getNextLedcChannel();

    private:
      static GpioState state;
    };
  }
}

#endif