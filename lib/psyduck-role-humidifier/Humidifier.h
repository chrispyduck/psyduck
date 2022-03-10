#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include <Psyduck.h>
#include "HumidifierSettings.h"

using psyduck::base::Logger;
using psyduck::sensors::ITemperatureAndHumidity;
using namespace psyduck::homie;
using namespace psyduck::gpio;

namespace psyduck
{
  namespace roles
  {
    namespace humidifier
    {
      struct HumidifierHardwareConfiguration
      {
        byte fanPwmPin;
        byte atomizerPwmPin;
        byte waterLevelLowSensorPin;
        byte waterValveOpenPin;
        long checkInterval = 1000;
      };

      

      struct HumidifierGpioState
      {
        DutyCycleOutput fan;
        DutyCycleOutput atomizer;

        bool waterLevelLowSensor;
        bool waterValveOpen;
      };

      class Humidifier
      {

      public:
        Humidifier(
            psyduck::Psyduck *main,
            ITemperatureAndHumidity *sensor,
            HumidifierHardwareConfiguration hardwareConfig);

        void check();

      private:
        Logger *logger = nullptr;
        HumidifierHardwareConfiguration hardwareConfiguration;
        HumidifierGpioState gpioState;
        ITemperatureAndHumidity *sensor = nullptr;
        HumidifierSettings settings;

        HomieNode *configNode = nullptr;
        HomieProperty *configDesiredHumidityProperty = nullptr;
        HomieProperty *configLowHumidityProperty = nullptr;
        HomieProperty *configFanIdleDutyCycleProperty = nullptr;
        HomieProperty *configFanHighDutyCycleProperty = nullptr;
        HomieProperty *configAtomizerMinDutyCycleProperty = nullptr;
        HomieProperty *configAtomizerMaxDutyCycleProperty = nullptr;

        HomieNode *stateNode = nullptr;
        HomieProperty *currentFanDutyCycleProperty = nullptr;
        HomieProperty *currentAtomizerDutyCycleProperty = nullptr;
        HomieProperty *currentWaterValveStatusProperty = nullptr;
        HomieProperty *currentWaterLevelLowProperty = nullptr;

        void checkWaterValve();
        void checkFanSpeed();
        void checkAtomizer();

        float computeAtomizerDutyCycle();
        bool shouldWaterValveBeOpen();

        static bool timerTick(void *);

        long lastAtomizerActive = 0;
      };

    }

  }
}
#endif