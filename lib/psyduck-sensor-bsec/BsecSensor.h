#ifndef CHRISPYDUCK_SENSOR_BSEC_H
#define CHRISPYDUCK_SENSOR_BSEC_H

#include <Psyduck.h>
#include <bsec.h>
#include "BsecSettings.h"
#include <Wire.h>
#include <SPI.h>

using psyduck::Psyduck;
using psyduck::base::Logger;
using psyduck::sensors::ITemperatureAndHumidity;
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
        BsecSensor(Psyduck *psyduck, TwoWire& i2cBus);
        BsecSensor(Psyduck *psyduck, SPIClass& spiBus, byte chipSelectPin);

        float getTemperature() override;
        float getHumidity() override;

        bool isReady();
        void read();

      private:
        void initCommon();
        void initHomie(Psyduck *psyduck);
        void initBsecSettings();
        void initBsecHardware(TwoWire& i2cBus);
        void initBsecHardware(SPIClass& spiBus, byte chipSelectPin);

        Bsec bsec;
        Logger *logger = nullptr;
        short maxAccuracy = 0;
        unsigned long lastSave = 0;
        bool ready = false;

        const int SENSOR_READ_INTERVAL = 10000;
        const int SENSOR_STATE_SAVE_INTERVAL_MINUTES = 300;

        void checkBsecStatus();

        static bool sensorTimerTick(void* ref);
        static void executeSensorTask(void* ref);

        BsecSettings settings;

        HomieNode *bsecNode = nullptr;
        HomieProperty *temperatureProperty = nullptr;
        HomieProperty *humidityProperty = nullptr;
        HomieProperty *pressureProperty = nullptr;
        HomieProperty *co2Property = nullptr;
        HomieProperty *co2AccuracyProperty = nullptr;
        HomieProperty *vocProperty = nullptr;
        HomieProperty *vocAccuracyProperty = nullptr;
        HomieProperty *iaqProperty = nullptr;
        HomieProperty *iaqAccuracyProperty = nullptr;
      };
    }
  }
}

#endif