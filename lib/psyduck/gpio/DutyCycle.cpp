#include "DutyCycle.h"

namespace psyduck
{
  namespace gpio
  {
    DutyCycle::DutyCycle()
    {
      this->set(0.0);
    }

    DutyCycle::DutyCycle(double value)
    {
      this->set(value);
    }

    DutyCycle::DutyCycle(uint32_t value)
    {
      this->set(value);
    }

    void DutyCycle::set(double value)
    {
      if (value < 0.0)
        value = 0.0;
      else if (value > 1.0)
        value = 1.0;
      this->decimalValue = value;
      this->value = (int)(value * 255);
      this->onSet();
    }

    void DutyCycle::set(uint32_t value)
    {
      this->value = value;
      this->decimalValue = (double)(value / 255.0);
      this->onSet();
    }

    double DutyCycle::getDecimal()
    {
      return this->decimalValue;
    }

    uint32_t DutyCycle::getRaw()
    {
      return this->value;
    }
  }
}