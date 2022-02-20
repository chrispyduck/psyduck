#ifndef CHRISPYDUCK_TEMPANDHUM_H
#define CHRISPYDUCK_TEMPANDHUM_H

#include "ITemperature.h"
#include "IHumidity.h"

namespace psyduck
{
  namespace sensors
  {
    class ITemperatureAndHumidity : public ITemperature, IHumidity
    {
    public:
      virtual float getTemperature() = 0;
      virtual float getHumidity() = 0;
    };
  }
}

#endif