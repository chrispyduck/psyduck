
#include "SwitchArray.h"

using psyduck::base::Logger;
using namespace psyduck::homie;

namespace psyduck
{
  namespace roles
  {
    namespace switcharray
    {
      SwitchArray::SwitchArray(SwitchArrayHardwareConfiguration hardwareConfiguration)
      {
        this->logger = new Logger(__FILE__);
        this->hardwareConfiguration = hardwareConfiguration;
      }

      void SwitchArray::init(psyduck::Psyduck *main)
      {
        this->logger->trace("Initializing raw GPIO pins");
        pinMode(hardwareConfiguration.channel1, OUTPUT);
        pinMode(hardwareConfiguration.channel2, OUTPUT);
        pinMode(hardwareConfiguration.channel3, OUTPUT);
        pinMode(hardwareConfiguration.channel4, OUTPUT);

        this->logger->trace("Initializing Homie members");
        this->stateNode = new HomieNode(main->getHomieDevice(), "switchArray", "SwitchArray State", "switchArray");
        this->channel1Property = HomieProperty::boolean(
            this->stateNode,
            "channel1",
            "Channel 1",
            false,
            [this](const String &value)
            {
              this->setChannel(this->hardwareConfiguration.channel1, value, this->channel1Property);
            });
        this->channel2Property = HomieProperty::boolean(
            this->stateNode,
            "channel2",
            "Channel 2",
            false,
            [this](const String &value)
            {
              this->setChannel(this->hardwareConfiguration.channel2, value, this->channel2Property);
            });
        this->channel3Property = HomieProperty::boolean(
            this->stateNode,
            "channel3",
            "Channel 3",
            false,
            [this](const String &value)
            {
              this->setChannel(this->hardwareConfiguration.channel3, value, this->channel3Property);
            });
        this->channel4Property = HomieProperty::boolean(
            this->stateNode,
            "channel4",
            "Channel 4",
            false,
            [this](const String &value)
            {
              this->setChannel(this->hardwareConfiguration.channel4, value, this->channel4Property);
            });
      }

      void SwitchArray::setChannel(const GpioPinId &pin, const String &value, HomieProperty *property)
      {
        if (value == "true")
        {
          digitalWrite(pin, HIGH);
          property->setValue("true");
        }
        else
        {
          digitalWrite(pin, LOW);
          property->setValue("false");
        }
      }
    }
  }
}