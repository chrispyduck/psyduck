#ifndef THERMOSTAT_SETTINGS_H
#define THERMOSTAT_SETTINGS_H

#include <psyduck-base.h>

using namespace psyduck::base;

namespace psyduck
{
  namespace roles
  {
    namespace thermostat
    {
      struct ThermostatSettingsData
      {
        char header[6];

        float setPoint = 71;
        bool fanOn = false;
        float fanDurationAfterHeat = 5;
        float safetyMinimumTemperature = 40;
        float safetyMaximumTemperature = 80;
        float temperatureMargin = 0.5;
      };

      class ThermostatSettings : public SettingsInstance<ThermostatSettingsData>
      {
      public:
        float getSetPoint();
        float setSetPoint(float value);

        bool getFanOn();
        bool setFanOn(bool value);

        float getFanDurationAfterHeat();
        float setFanDurationAfterHeat(float value);

        float getSafetyMinimumTemperature();
        float setSafetyMinimumTemperature(float value);

        float getSafetyMaximumTemperature();
        float setSafetyMaximumTemperature(float value);

        float getTemperatureMargin();
        float setTemperatureMargin(float value);
      };
    }
  }
}

#endif