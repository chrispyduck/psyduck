#include "psyduck-sensor-aht20.h"

namespace psyduck
{
  namespace sensors
  {
    bool timerTick(void *ref)
    {
      AHT20 *instance = reinterpret_cast<AHT20 *>(ref);
      instance->logger->debug("timerTick");
      instance->read();
      return true;
    }

    AHT20::AHT20(Psyduck *psyduck)
    {
      this->logger = new Logger(__FILE__);
      this->psyduck = psyduck;
      if (!this->aht.begin())
      {
        this->logger->error(F("Failed to locate AHTx0 on I2C bus"));
        // todo: move into fault state
      }

      this->aht_temp = aht.getTemperatureSensor();
      this->aht_temp->printSensorDetails();

      this->aht_humidity = aht.getHumiditySensor();
      this->aht_humidity->printSensorDetails();

      this->temp = 0;
      this->hum = 0;

      auto task = Timers::every(10000, timerTick, this);
      this->logger->debug("timer task: %d", task);

      this->node = new HomieNode(psyduck->getHomieDevice(), "environment", "Environment", "environment");

      this->temperatureProperty = new HomieProperty(this->node, "temperature", "Temperature", "float");
      this->temperatureProperty->setUnit("°F");
      this->temperatureProperty->setFormat("0:100");

      this->humidityProperty = new HomieProperty(this->node, "humidity", "Humidity", "float");
      this->humidityProperty->setUnit("%");
      this->humidityProperty->setFormat("0:100");
    }

    void AHT20::read()
    {
      this->logger->debug("Reading AHTx0 temperature and humidity; status=%d", this->aht.getStatus());

      sensors_event_t humidity;
      sensors_event_t temp;
      this->aht_humidity->getEvent(&humidity);
      this->aht_temp->getEvent(&temp);

      this->temp = temp.temperature * 9 / 5 + 32;
      this->hum = humidity.relative_humidity;
      this->logger->info(F("temp = %f, hum = %f"),
                         this->temp,
                         this->hum);

      if (millis() - this->lastReport > 30000)
      {

        this->lastReport = millis();
        if (this->temp > -40 && this->temp < 85)
          this->temperatureProperty->setValue(this->temp, 1);
        if (this->hum > 0 && this->hum < 100)
          this->humidityProperty->setValue(this->hum, 0);
        this->logger->info(F("temp = %s, hum = %s"),
                           this->temperatureProperty->getValue(),
                           this->humidityProperty->getValue());
      }
    }

    float AHT20::getTemperature()
    {
      return this->temp;
    }

    float AHT20::getHumidity()
    {
      return this->hum;
    }
  }
}