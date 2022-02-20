#ifndef CHRISPYDUCK_SENSOR_BSEC_H
#define CHRISPYDUCK_SENSOR_BSEC_H

#include <Psyduck.h>
#include <bsec.h>
#include "BsecSettings.h"

using psyduck::base::Logger;
using psyduck::sensors::ITemperatureAndHumidity;
using psyduck::Psyduck;
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
        BsecSensor(Psyduck *psyduck);

        float getTemperature() override;
        float getHumidity() override;

        friend bool sensorTimerTick(void *);

      private:
        Bsec bsec;
        Logger *logger;
        short maxAccuracy = 0;
        unsigned long lastSave = 0;

        const int SENSOR_READ_INTERVAL = 3000;
        const int SENSOR_STATE_SAVE_INTERVAL_MINUTES = 300;

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