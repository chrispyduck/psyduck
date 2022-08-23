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
      void init(GpioPinId pin, double frequency = 5000, const char* name = nullptr);

      // tests the full range of possible outputs from 0 to max over the specified time, then resets the value to 0
      void test(int timeMs);

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