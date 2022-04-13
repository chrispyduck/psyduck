#include "DutyCycleOutput.h"
#include "GPIO.h"
#include <str.h>
#include "../base/psyduck-base.h"

namespace psyduck
{
  namespace gpio
  {
    void DutyCycleOutput::init(GpioPinId pin, double frequency, const char *name)
    {
      if (this->pin != UINT8_MAX)
      {
        throw std::exception();
      }

      char pinName[4];
      this->logger = new psyduck::base::Logger(
          name == nullptr ? __FILE__ : name,
          itoa(pin, pinName, 10));
      this->logger->trace("Initializing");

      this->pin = pin;
      this->ledcChannel = GPIO::getNextLedcChannel();

      // this->logger->trace("ledcSetup(%u, %.0f, 8)", this->ledcChannel, frequency);
      ledcSetup(this->ledcChannel, frequency, 8u);

      // this->logger->trace("ledcAttachPin(%u, %u)", this->ledcChannel, this->pin);
      ledcAttachPin(this->pin, this->ledcChannel);

      this->logger->debug("Initialized using ledc channel %u (%.0fHz, %ubit)", this->ledcChannel, frequency, 8u);

      this->onSet();
    }

    void DutyCycleOutput::onSet()
    {
      this->assertInitialized();
      ledcWrite(this->ledcChannel, this->getRaw());
      this->logger->debug("Set value of channel %u to %f (0x%X)", this->ledcChannel, this->getDecimal(), this->getRaw());
    }

    inline void DutyCycleOutput::assertInitialized()
    {
      if (this->pin == UINT8_MAX || this->logger == nullptr)
        throw std::exception();
    }

    struct SelfTestStep
    {
      DutyCycleOutput *output;
      uint32_t value;
      uint32_t maxValue;
      double delay;
    };

    bool selfTestStep(void *data)
    {
      SelfTestStep *step = static_cast<SelfTestStep *>(data);
      if (step->value < step->maxValue)
      {
        step->value++;
        step->output->set(step->value);
        psyduck::base::Timers::in(step->delay, selfTestStep, step);
      }
      else
      {
        step->output->set(0.0);
        delete step;
      }
      return false;
    }

    void DutyCycleOutput::test(int timeMs)
    {

      this->logger->info("Testing channel %u over the next %ums", this->ledcChannel, timeMs);
      SelfTestStep *step = new SelfTestStep();
      step->output = this;
      step->delay = timeMs / 255.0;
      step->value = 0;
      step->maxValue = 255;
      selfTestStep(step);
    }
  }
}