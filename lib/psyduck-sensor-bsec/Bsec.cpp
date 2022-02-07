#include "Bsec.h"
#include <math.h>
#include <Wire.h>

using namespace psyduck::homie;

namespace psyduck
{
  namespace sensors
  {
    namespace bsec
    {

      const uint8_t bsec_config_iaq[] = {
#include "config/generic_33v_3s_4d/bsec_iaq.txt"
      };

      bsec_virtual_sensor_t sensorList[] = {
          BSEC_OUTPUT_RAW_TEMPERATURE,
          BSEC_OUTPUT_RAW_HUMIDITY,
          BSEC_OUTPUT_RAW_PRESSURE,
          BSEC_OUTPUT_RAW_GAS,
          BSEC_OUTPUT_STATIC_IAQ,
          BSEC_OUTPUT_CO2_EQUIVALENT,
          BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
          BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
          BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
      };

      BsecSensor::BsecSensor(Timer<> *timer, HomieDevice *device)
      {
        this->bsecNode = new HomieNode(device, "environment", "Environment", "environment");

        this->temperatureProperty = new HomieProperty(this->bsecNode, "temperature", "Temperature", "float");
        this->temperatureProperty->setUnit("°F");
        this->temperatureProperty->setFormat("0:100");

        this->humidityProperty = new HomieProperty(this->bsecNode, "humidity", "Humidity", "float");
        this->humidityProperty->setUnit("%");
        this->humidityProperty->setFormat("0:100");

        this->pressureProperty = new HomieProperty(this->bsecNode, "pressure", "Pressure", "float");
        this->pressureProperty->setUnit("hPa");

        this->co2Property = new HomieProperty(this->bsecNode, "co2", "CO2 Equivalent", "float");
        this->co2Property->setUnit("ppm");
        this->co2AccuracyProperty = new HomieProperty(this->bsecNode, "co2Accuracy", "CO2 Equivalent Accuracy", "enum");
        this->co2AccuracyProperty->setFormat("UNRELIABLE,LOW,MEDIUM,HIGH");

        this->vocProperty = new HomieProperty(this->bsecNode, "voc", "Breath VOC Equivalent", "float");
        this->vocProperty->setUnit("ppm");
        this->vocAccuracyProperty = new HomieProperty(this->bsecNode, "vocAccuracy", "Breath VOC Equivalent Accuracy", "enum");
        this->vocAccuracyProperty->setFormat("UNRELIABLE,LOW,MEDIUM,HIGH");

        this->iaqProperty = new HomieProperty(this->bsecNode, "iaq", "Air Quality", "integer");
        this->iaqProperty->setUnit("IAQ");
        this->iaqProperty->setFormat("0:500");
        this->iaqAccuracyProperty = new HomieProperty(this->bsecNode, "iaqAccuracy", "Air Quality Accuracy", "enum");
        this->iaqAccuracyProperty->setFormat("UNRELIABLE,LOW,MEDIUM,HIGH");

        timer->every(SENSOR_READ_INTERVAL, &sensorTimerTick);

        Log.noticeln("BSEC library version %d.%d.%d.%d",
                     this->bsec.version.major,
                     this->bsec.version.minor,
                     this->bsec.version.major_bugfix,
                     this->bsec.version.minor_bugfix);

        this->bsec.begin(BME680_I2C_ADDR_SECONDARY, Wire);
        this->checkBsecStatus();

        this->bsec.setConfig(bsec_config_iaq);
        this->checkBsecStatus();

        uint8_t *defaultState = new uint8_t[BSEC_MAX_STATE_BLOB_SIZE];
        this->bsec.getState(defaultState);
        uint8_t *state = this->settings.getBsecState();
        this->bsec.setState(state);
        if (this->bsec.status != 0)
        {
          Log.warningln("Failed to load persisted state: %d", this->bsec.status);
          this->bsec.setState(defaultState);
          this->checkBsecStatus();
          std::copy(defaultState, defaultState + BSEC_MAX_STATE_BLOB_SIZE, state);
          this->settings.save();
        }
        delete defaultState;

        this->bsec.updateSubscription(sensorList, sizeof(sensorList) / sizeof(bsec_virtual_sensor_t), BSEC_SAMPLE_RATE_LP);
        this->checkBsecStatus();
      }

      void Bsec::read()
      {
        auto currentTemp = sensors.bsec.temperature * 9 / 5 + 32;
        auto currentHumidity = sensors.bsec.humidity;

        this->temperatureProperty->setValue(currentTemp);
        this->humidityProperty->setValue(currentHumidity);
        this->pressureProperty->setValue(sensors.bsec.pressure);
        this->co2Property->setValue(sensors.bsec.co2Equivalent);
        this->co2AccuracyProperty->setValue(formatAccuracyValue(sensors.bsec.co2Accuracy));
        this->vocProperty->setValue(sensors.bsec.breathVocEquivalent);
        this->vocAccuracyProperty->setValue(formatAccuracyValue(sensors.bsec.breathVocAccuracy));
        this->iaqProperty->setValue(sensors.bsec.staticIaq);
        this->iaqAccuracyProperty->setValue(formatAccuracyValue(sensors.bsec.staticIaqAccuracy));
      }

      bool sensorTimerTick(void *)
      {
        if (!sensors.bsec.run())
        {
          checkBsecStatus();
        }
        else
        {
          // read successful; print info
          Log.noticeln("ts=%d: temp=%FC %FF, raw temp=%FC %FF, hum=%F, raw hum=%F, pres=%F, staticIaq=%F (acc=%d), co2=%F (acc=%d), bvoc=%F (acc=%d)",
                       sensors.bsec.outputTimestamp,
                       sensors.bsec.temperature,
                       sensors.bsec.temperature * 9 / 5 + 32,
                       sensors.bsec.rawTemperature,
                       sensors.bsec.rawTemperature * 9 / 5 + 32,
                       sensors.bsec.humidity,
                       sensors.bsec.rawHumidity,
                       sensors.bsec.pressure,
                       sensors.bsec.staticIaq,
                       sensors.bsec.staticIaqAccuracy,
                       sensors.bsec.co2Equivalent,
                       sensors.bsec.co2Accuracy,
                       sensors.bsec.breathVocEquivalent,
                       sensors.bsec.breathVocAccuracy);

          if ((sensors.bsec.staticIaqAccuracy == 3 && (sensors.maxAccuracy < 3) || (millis() - sensors.lastSave) / 60000 > SENSOR_STATE_SAVE_INTERVAL_MINUTES))
          {
            Log.noticeln("Saving BSEC algorithm state");
            uint8_t *state = settings.getBsecState();
            sensors.bsec.getState(state);
            settings.save();
            sensors.lastSave = millis();
          }

          if (sensors.maxAccuracy < sensors.bsec.staticIaqAccuracy)
          {
            Log.noticeln("Achieved new maximum accuracy: %d", sensors.bsec.staticIaqAccuracy);
            sensors.maxAccuracy = sensors.bsec.staticIaqAccuracy;
          }
        }
        return true;
      }

      void BsecSensor::checkBsecStatus()
      {
        if (this->bsec.status < BSEC_OK)
        {
          Serial.println("BSEC error code : " + String(this->bsec.status));
          gpio.activateFaultState(); /* Halt in case of failure */
        }
        else if (this->bsec.status > BSEC_OK)
        {
          Serial.println("BSEC warning code : " + String(this->bsec.status));
        }

        if (this->bsec.bme680Status < BME680_OK)
        {
          Serial.println("BME68X error code : " + String(this->bsec.bme680Status));
          gpio.activateFaultState(); /* Halt in case of failure */
        }
        else if (this->bsec.bme680Status > BME680_OK)
        {
          Serial.println("BME68X warning code : " + String(this->bsec.bme680Status));
        }
      }
    }
  }
}