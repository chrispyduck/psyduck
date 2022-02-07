#ifndef CHRISPYDUCK_SENSOR_BSEC_H
#define CHRISPYDUCK_SENSOR_BSEC_H

#include <psyduck-base.h>
#include <psyduck-homie.h>
#include <psyduck-sensor-interfaces.h>
#include <bsec.h>
#include <arduino-timer.h>
#include "BsecSettings.h"

using namespace psyduck::homie;

namespace psyduck
{
  namespace sensors
  {
    namespace bsec
    {

      class BsecSensor : public ITemperatureAndHumidity
      {
      public:
        BsecSensor(Timer<> *timer, HomieDevice *device);

        Bsec bsec;
        friend bool sensorTimerTick(void *);

      private:
        void read();
        void checkBsecStatus();

        BsecSettings settings;

        HomieNode *bsecNode;
        HomieProperty *temperatureProperty;
        HomieProperty *humidityProperty;
        HomieProperty *pressureProperty;
        HomieProperty *co2Property;
        HomieProperty *co2AccuracyProperty;
        HomieProperty *vocProperty;
        HomieProperty *vocAccuracyProperty;
        HomieProperty *iaqProperty;
        HomieProperty *iaqAccuracyProperty;
      };
    }
  }
}

#endif