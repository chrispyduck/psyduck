#include "DutyCycle.h"

namespace psyduck
{
  namespace gpio
  {
    DutyCycle::DutyCycle() {
      this->set(0.0f);
    }

    DutyCycle::DutyCycle(float value) {
      this->set(value);
    }

    DutyCycle::DutyCycle(byte value) {
      this->set(value);
    }

    void DutyCycle::set(float value)
    {
      if (value < 0.0f)
        value = 0.0f;
      else if (value > 1.0f)
        value = 1.0f;
      this->floatValue = value;
      this->byteValue = (byte)(value * 255);
    }

    void DutyCycle::set(byte value)
    {
      if (value < 0)
        value = 0;
      else if (value > 255)
        value = 255;
      this->byteValue = value;
      this->floatValue = (float)(value / 255.0f);
    }

    float DutyCycle::getFloat() {
      return this->floatValue;
    }

    byte DutyCycle::getByte() {
      return this->byteValue;
    }
  }
}