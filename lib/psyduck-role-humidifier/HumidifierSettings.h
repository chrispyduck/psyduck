#ifndef THERMOSTAT_SETTINGS_H
#define THERMOSTAT_SETTINGS_H

#include <Psyduck.h>

using namespace psyduck::base;

namespace psyduck
{
  namespace roles
  {
    namespace humidifier
    {
      struct __attribute__((packed)) HumidifierSettingsData
      {
        char header[6];

        float desiredHumidity = 45;
        float lowHumidity = 20;
        float fanIdleDutyCycle = 0.5;
        float fanHighDutyCycle = 1.0;
        float atomizerMinDutyCycle = 0.4;
        float atomizerMaxDutyCycle = 1.0;
      };

      class HumidifierSettings : public SettingsInstance<HumidifierSettingsData>
      {
      public:
        HumidifierSettings();
        
        float getDesiredHumidity();
        float setDesiredHumidity(float value);

        float getLowHumidity();
        float setLowHumidity(float value);

        float getFanIdleDutyCycle();
        float setFanIdleDutyCycle(float value);

        float getFanHighDutyCycle();
        float setFanHighDutyCycle(float value);

        float getAtomizerMinDutyCycle();
        float setAtomizerMinDutyCycle(float value);

        float getAtomizerMaxDutyCycle();
        float setAtomizerMaxDutyCycle(float value);
      };
    }
  }
}

#endif