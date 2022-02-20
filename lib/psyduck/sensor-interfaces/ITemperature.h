#ifndef CHRISPYDUCK_TEMP_H
#define CHRISPYDUCK_TEMP_H

namespace psyduck
{
  namespace sensors
  {
    class ITemperature
    {
    public:
      virtual float getTemperature() = 0;
    };
  }
}

#endif