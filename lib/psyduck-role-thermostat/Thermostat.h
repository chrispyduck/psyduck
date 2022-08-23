#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include <Psyduck.h>
#include "ThermostatSettings.h"

using psyduck::base::Logger;
using psyduck::sensors::ITemperatureAndHumidity;
using namespace psyduck::homie;
using namespace psyduck::gpio;

namespace psyduck
{
  namespace roles
  {
    namespace thermostat
    {
      struct ThermostatHardwareConfiguration {
        GpioLatchingSwitch heatOn;
        GpioLatchingSwitch fanOn;
      };

      struct ThermostatGpioState {
        bool heatOn;
        bool fanOn;
      };

      class Thermostat
      {

      public:
        Thermostat(
          psyduck::Psyduck* main, 
          ITemperatureAndHumidity *sensor,
          ThermostatHardwareConfiguration hardwareConfig);

        void checkThermostat();

      private:
        ThermostatHardwareConfiguration hardwareConfiguration;
        ThermostatGpioState gpioState;
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

        void setFanOn(bool desiredState);
        void setHeatOn(bool desiredState);
      };

    }

  }
}
#endif