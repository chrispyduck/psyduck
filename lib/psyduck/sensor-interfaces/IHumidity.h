#ifndef CHRISPYDUCK_HUM_H
#define CHRISPYDUCK_HUM_H

namespace psyduck
{
  namespace sensors
  {
    class IHumidity
    {
    public:
      virtual float getHumidity() = 0;
    };
  }
}

#endif