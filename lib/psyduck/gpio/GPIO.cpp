#include "GPIO.h"

namespace psyduck
{
  namespace gpio
  {
    void GPIO::setSwitch(GpioLatchingSwitch &config, bool desiredValue)
    {
      digitalWrite(desiredValue
                       ? config.pinOn
                       : config.pinOff,
                   HIGH);
      delay(MOMENTARY_SWITCH_DELAY);
      digitalWrite(desiredValue
                       ? config.pinOn
                       : config.pinOff,
                   LOW);
    }

    void GPIO::setSwitch(GpioNonLatchingSwitch &config, bool desiredValue)
    {
      digitalWrite(config.pinOn, desiredValue ? HIGH : LOW);
    }
  }
}