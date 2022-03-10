#pragma once
#include "DutyCycle.h"
#include "../base/psyduck-base.h"

namespace psyduck
{
  namespace gpio
  {
    class DutyCycleOutput : public DutyCycle
    {
    public:
      void init(uint8_t pin);

    protected:
      psyduck::base::Logger* logger;

      virtual void onSet() override;

    private:
      uint8_t pin = UINT8_MAX;
      uint8_t ledcChannel = UINT8_MAX;

      void assertInitialized();
    };
  }
}