#include "BsecSensor.h"
#include <math.h>
#include <Wire.h>

using psyduck::Psyduck;
using namespace psyduck::homie;

namespace psyduck
{
  namespace sensors
  {
    namespace bsec
    {
      const char *formatAccuracyValue(uint8_t value)
      {
        if (value == 0)
        {
          return "UNRELIABLE";
        }
        else if (value == 1)
        {
          return "LOW";
        }
        else if (value == 2)
        {
          return "MEDIUM";
        }
        else if (value == 3)
        {
          return "HIGH";
        }
        return "UNKNOWN";
      }

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

          //BSEC_OUTPUT_RUN_IN_STATUS,
          //BSEC_OUTPUT_STABILIZATION_STATUS,
          //BSEC_OUTPUT_GAS_PERCENTAGE,
      };

      BsecSensor::BsecSensor(Psyduck *psyduck, TwoWire i2cBus)
      {
        this->initCommon();
        this->initHomie(psyduck);
        this->initBsecHardware(i2cBus);
        this->initBsecSettings();
      }

      BsecSensor::BsecSensor(Psyduck *psyduck, SPIClass spiBus, byte chipSelectPin)
      {
        this->initCommon();
        this->initHomie(psyduck);
        this->initBsecHardware(spiBus, chipSelectPin);
        this->initBsecSettings();
      }

      bool BsecSensor::isReady()
      {
        return this->ready;
      }

      void BsecSensor::initCommon()
      {
        this->logger = new Logger(__FILE__);
        //Timers::every(SENSOR_READ_INTERVAL, &BsecSensor::sensorTimerTick, this);
      }

      void BsecSensor::initHomie(Psyduck *psyduck)
      {
        if (psyduck == nullptr)
        {
          return;
        }
        this->bsecNode = new HomieNode(psyduck->getHomieDevice(), "environment", "Environment", "environment");

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
      }

      void BsecSensor::initBsecHardware(TwoWire i2cBus)
      {
        this->bsec.begin(BME680_I2C_ADDR_SECONDARY, i2cBus);
        this->checkBsecStatus();
      }

      void BsecSensor::initBsecHardware(SPIClass spiBus, byte chipSelectPin)
      {
        this->bsec.begin(chipSelectPin, spiBus);
        this->checkBsecStatus();
      }

      IRAM_ATTR void BsecSensor::initBsecSettings()
      {
        this->logger->info("BSEC library version %d.%d.%d.%d",
                           this->bsec.version.major,
                           this->bsec.version.minor,
                           this->bsec.version.major_bugfix,
                           this->bsec.version.minor_bugfix);

        this->bsec.setConfig(bsec_config_iaq);
        this->checkBsecStatus();

        //uint8_t *defaultState = new uint8_t[BSEC_MAX_STATE_BLOB_SIZE];
        //this->bsec.getState(defaultState);
        //uint8_t *state = this->settings.getBsecState();
        //this->bsec.setState(state);
        if (this->bsec.status != 0)
        {
          this->logger->warn("Failed to load persisted state: %d", this->bsec.status);
          //this->bsec.setState(defaultState);
          this->checkBsecStatus();
          //this->bsec.
          //memcpy(state, defaultState, BSEC_MAX_STATE_BLOB_SIZE);
          //this->settings.save();
        }
        //delete defaultState;

        this->bsec.updateSubscription(sensorList, sizeof(sensorList) / sizeof(bsec_virtual_sensor_t), BSEC_SAMPLE_RATE_CONTINUOUS);
        this->checkBsecStatus();
      }

      IRAM_ATTR void BsecSensor::read()
      {
        this->logger->debug("Read(): starting");
        if (!this->bsec.run())
        {
          this->logger->debug("Read failed");
          this->checkBsecStatus();
        }
        else
        {
          // read successful; print info
          this->logger->info("ts=%d: temp=%FC %FF, raw temp=%FC %FF, hum=%F, raw hum=%F, pres=%F, staticIaq=%F (acc=%d), co2=%F (acc=%d), bvoc=%F (acc=%d)",
                             this->bsec.outputTimestamp,
                             this->bsec.temperature,
                             this->bsec.temperature * 9 / 5 + 32,
                             this->bsec.rawTemperature,
                             this->bsec.rawTemperature * 9 / 5 + 32,
                             this->bsec.humidity,
                             this->bsec.rawHumidity,
                             this->bsec.pressure,
                             this->bsec.staticIaq,
                             this->bsec.staticIaqAccuracy,
                             this->bsec.co2Equivalent,
                             this->bsec.co2Accuracy,
                             this->bsec.breathVocEquivalent,
                             this->bsec.breathVocAccuracy);

          if ((this->bsec.staticIaqAccuracy == 3 && ((this->maxAccuracy < 3) || (millis() - this->lastSave) / 60000 > SENSOR_STATE_SAVE_INTERVAL_MINUTES)))
          {
            this->logger->info("Saving BSEC algorithm state");
            uint8_t *state = settings.getBsecState();
            this->bsec.getState(state);
            settings.save();
            this->lastSave = millis();
          }

          if (this->maxAccuracy < this->bsec.staticIaqAccuracy)
          {
            this->logger->info("Achieved new maximum accuracy: %d", this->bsec.staticIaqAccuracy);
            this->maxAccuracy = this->bsec.staticIaqAccuracy;
          }

          auto currentTemp = this->bsec.temperature * 9 / 5 + 32;
          auto currentHumidity = this->bsec.humidity;

          if (this->bsecNode != nullptr)
          {
            this->temperatureProperty->setValue(currentTemp);
            this->humidityProperty->setValue(currentHumidity);
            this->pressureProperty->setValue(this->bsec.pressure);
            this->co2Property->setValue(this->bsec.co2Equivalent);
            this->co2AccuracyProperty->setValue(formatAccuracyValue(this->bsec.co2Accuracy));
            this->vocProperty->setValue(this->bsec.breathVocEquivalent);
            this->vocAccuracyProperty->setValue(formatAccuracyValue(this->bsec.breathVocAccuracy));
            this->iaqProperty->setValue(this->bsec.staticIaq);
            this->iaqAccuracyProperty->setValue(formatAccuracyValue(this->bsec.staticIaqAccuracy));
          }
        }
        this->logger->debug("Read(): exiting");
      }

      bool BsecSensor::sensorTimerTick(void *ref)
      {
        BsecSensor *sensor = static_cast<BsecSensor *>(ref);
        sensor->read();
        return true;
      }

      void BsecSensor::checkBsecStatus()
      {
        if (this->bsec.status < BSEC_OK)
        {
          this->logger->error("BSEC error code: %d", this->bsec.status);
          this->ready = false;
          // gpio.activateFaultState(); /* Halt in case of failure */
        }
        else if (this->bsec.status > BSEC_OK)
        {
          this->logger->error("BSEC warning code: %d", this->bsec.status);
        }
        else
        {
          this->ready = true;
        }

        switch (this->bsec.bme680Status)
        {
        case BME680_OK:
          this->ready = true;
          break;

        // errors
        case BME680_E_NULL_PTR:
          this->logger->error("BME68X error code: BME680_E_NULL_PTR");
          this->ready = false;
          break;

        case BME680_E_COM_FAIL:
          this->logger->error("BME68X error code: BME680_E_COM_FAIL");
          this->ready = false;
          break;

        case BME680_E_DEV_NOT_FOUND:
          this->logger->error("BME68X error code: BME680_E_DEV_NOT_FOUND");
          this->ready = false;
          break;

        case BME680_E_INVALID_LENGTH:
          this->logger->error("BME68X error code: BME680_E_INVALID_LENGTH");
          this->ready = false;
          break;

          // warnings
        case BME680_W_DEFINE_PWR_MODE:
          this->logger->warn("BME68X warning code: BME680_W_DEFINE_PWE_MODE");
          break;
        case BME680_W_NO_NEW_DATA:
          this->logger->warn("BME68X warning code: BME680_W_NO_NEW_DATA");
          break;
        default:
          if (this->bsec.bme680Status < BME680_OK)
          {
            this->logger->error("Unknown BME68X error code: %d", this->bsec.bme680Status);
            this->ready = false;

            // gpio.activateFaultState(); /* Halt in case of failure */
          }
          else
          {
            this->logger->warn("Unknown BME68X warning code: %d", this->bsec.bme680Status);
          }
        }
      }

      float BsecSensor::getTemperature()
      {
        return this->bsec.temperature;
      }
      float BsecSensor::getHumidity()
      {
        return this->bsec.humidity;
      }
    }
  }
}