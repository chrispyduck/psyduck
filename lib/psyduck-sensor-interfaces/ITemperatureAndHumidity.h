#ifndef CHRISPYDUCK_TEMPANDHUM_H
#define CHRISPYDUCK_TEMPANDHUM_H

namespace psyduck
{
  namespace sensors
  {
    class ITemperatureAndHumidity
    {
    public:
      virtual float getTemperature() = 0;
      virtual float getHumidity() = 0;
    };
  }
}

#endif