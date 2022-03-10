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
      DutyCycle(float value);
      DutyCycle(byte value);

      float getFloat();
      byte getByte();
      void set(float value);
      void set(byte value);

    protected:
      virtual void onSet() { };

    private:
      float floatValue;
      byte byteValue;
    };
  }
}