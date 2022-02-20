#include "SpeedController.h"

namespace psyduck
{
  namespace fans
  {
    SpeedController::SpeedController(Psyduck *psyduck, SpeedControllerConfiguration config, ITemperature *temperatureSensor)
    {
      this->logger = new Logger(__FILE__);
      this->config = config;
      this->temperatureSensor = temperatureSensor;

      this->node = new HomieNode(psyduck->getHomieDevice(), "fans", "Fans", "Fan Speed Monitor and Controller");
      this->dutyCycleProperty = new HomieProperty(this->node, "duty-cycle", "Duty Cycle", "float");
      this->dutyCycleProperty->setUnit("%");
      this->dutyCycleProperty->setFormat("0:100");

      ledcAttachPin(config.pwmGpioPin, 1);
      ledcSetup(1, 16000, 8);

      Timers::every(30000, SpeedController::tick, this);
    }

    bool SpeedController::tick(void* ref) {
      SpeedController* instance = static_cast<SpeedController*>(ref);
      instance->computeDutyCycle();
      return true;
    }

    void SpeedController::computeDutyCycle()
    {
      float temp = this->temperatureSensor->getTemperature();
      if (temp < 0 || temp > 185)
      {
        this->logger->warn("Temperature value of %f is out of range. Ignoring.", temp);
        temp = 185;
      }

      if (temp < this->config.minSpeedTemperature)
        this->dutyCycle = this->config.minimumDutyCycle;
      else if (temp > this->config.maxSpeedTemperature)
        this->dutyCycle = this->config.maximumDutyCycle;
      else
        this->dutyCycle = ((temp - this->config.minSpeedTemperature) / (this->config.maxSpeedTemperature - this->config.minSpeedTemperature)) * (this->config.maximumDutyCycle - this->config.minimumDutyCycle) + this->config.minimumDutyCycle;

      float dutyCyclePct = (float)this->dutyCycle / 255.0 * 100;
      this->logger->debug("temp=%f, duty cycle=%i, duty cycle pct=%f", temp, this->dutyCycle, dutyCyclePct);
      this->dutyCycleProperty->setValue(dutyCyclePct, 1);
      ledcWrite(1, this->dutyCycle);
    }

    HomieNode* SpeedController::getHomieNode() {
      return this->node;
    }
  }
}