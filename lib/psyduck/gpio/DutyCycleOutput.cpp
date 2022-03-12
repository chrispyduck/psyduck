#include "DutyCycleOutput.h"
#include "GPIO.h"
#include <str.h>

namespace psyduck
{
  namespace gpio
  {
    void DutyCycleOutput::init(GpioPinId pin)
    {
      if (this->pin != UINT8_MAX) {
        throw std::exception();
      }

      char pinName[4];
      this->logger = new psyduck::base::Logger(__FILE__, itoa(pin, pinName, 10));
      this->logger->trace("Initializing");
      
      this->pin = pin;

      this->ledcChannel = GPIO::getNextLedcChannel();
      ledcAttachPin(this->ledcChannel, this->pin);
      ledcSetup(this->ledcChannel, 1600, 8);      
      this->logger->debug("Initialized using ledc channel %d", this->ledcChannel);
      
      this->onSet();

      this->assertInitialized();
    }

    void DutyCycleOutput::onSet() {
      this->assertInitialized();
      ledcWrite(this->ledcChannel, this->getByte());
      this->logger->debug("Value set to %.2f", this->getFloat());
    }

    inline void DutyCycleOutput::assertInitialized() {
      if (this->pin == UINT8_MAX || this->logger == nullptr)
        throw std::exception();
    }
  }
}