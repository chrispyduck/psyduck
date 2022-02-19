#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include <psyduck-base.h>
#include <psyduck-homie.h>
#include <psyduck-sensor-interfaces.h>
#include "ThermostatSettings.h"

using psyduck::base::Logger;
using psyduck::sensors::ITemperatureAndHumidity;
using namespace psyduck::homie;

namespace psyduck
{
  namespace roles
  {
    namespace thermostat
    {

      class Thermostat
      {

      public:
        Thermostat(psyduck::base::Psyduck* main, ITemperatureAndHumidity *sensor);

        void checkThermostat();

      private:
        Logger *logger;
        ITemperatureAndHumidity *sensor;
        Timer<> *timer;
        ThermostatSettings settings;

        HomieNode *thermostatNode;
        HomieProperty *fanOnProperty;
        HomieProperty *heatOnProperty;
        HomieProperty *setPointProperty;
        HomieProperty *fanDurationAfterHeatProperty;
        HomieProperty *temperatureMarginProperty;
        HomieProperty *safetyMinimumTemperatureProperty;
        HomieProperty *safetyMaximumTemperatureProperty;
        
        bool firstRun = true;
        unsigned long int lastHeatOff = 0;
      };

    }

  }
}
#endif