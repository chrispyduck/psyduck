#pragma once
#include <Arduino.h>

namespace psyduck
{
  namespace gpio
  {
    class DutyCycle
    {
    public:
      DutyCycle();
      DutyCycle(double value);
      DutyCycle(uint32_t value);

      double getDecimal();
      uint32_t getRaw();
      void set(double value);
      void set(uint32_t value);

    protected:
      virtual void onSet() { };

    private:
      double decimalValue;
      uint32_t value;
    };
  }
}