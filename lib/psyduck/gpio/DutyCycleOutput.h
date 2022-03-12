#pragma once
#include "DutyCycle.h"
#include "GPIO.h"
#include "../base/psyduck-base.h"

namespace psyduck
{
  namespace gpio
  {
    class DutyCycleOutput : public DutyCycle
    {
    public:
      void init(GpioPinId pin);

    protected:
      psyduck::base::Logger* logger;

      virtual void onSet() override;

    private:
      GpioPinId pin = UINT8_MAX;
      uint8_t ledcChannel = UINT8_MAX;

      void assertInitialized();
    };
  }
}