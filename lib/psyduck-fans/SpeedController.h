#ifndef PSYDUCK_FAN_SPEEDCONTROLLER_H
#define PSYDUCK_FAN_SPEEDCONTROLLER_H

#include <Psyduck.h>
#include "Tachometer.h"

namespace psyduck
{
  namespace fans
  {
    struct SpeedControllerConfiguration {
      short pwmGpioPin;

      byte minimumDutyCycle = 0x0;
      byte maximumDutyCycle = 0xff;

      float minSpeedTemperature = 70.0;
      float maxSpeedTemperature = 85.0;
    };

    class SpeedController
    {
    public:
      SpeedController(Psyduck* psyduck, SpeedControllerConfiguration config, ITemperature* temperatureSensor);

      HomieNode* getHomieNode();
      void computeDutyCycle();

    private:
      Logger* logger;
      SpeedControllerConfiguration config;
      ITemperature* temperatureSensor;
      HomieNode* node;
      HomieProperty* dutyCycleProperty;
      byte dutyCycle;

      

      static bool tick(void*);
    };
  }
}

#endif