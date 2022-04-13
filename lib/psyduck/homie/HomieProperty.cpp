#include "homie-declarations.h"

using psyduck::base::Logger;

namespace psyduck
{
  namespace homie
  {

    HomieProperty::HomieProperty(HomieNode *node, const char *id, const char *name, const char *datatype)
    {
      this->id = id;
      this->name = name;
      this->type = datatype;
      this->node = node;

      this->mqttPath = new char[strlen(id) + strlen(node->getMqttPath()) + 3];
      strcpy(this->mqttPath, node->getMqttPath());
      strcat(this->mqttPath, this->id);

      this->setterTopic = new char[strlen(this->mqttPath) + 6];
      strcpy(this->setterTopic, this->mqttPath);
      strcat(this->setterTopic, "/set");

      this->logger = new Logger(this->mqttPath);

      node->registerProperty(this);
    }

    const char *HomieProperty::getName()
    {
      return this->name;
    }

    const char *HomieProperty::getId()
    {
      return this->id;
    }

    void HomieProperty::publish()
    {
      this->logger->debug("publishing");
      this->publishAttribute("$name", this->name);
      this->publishAttribute("$datatype", this->type);
      this->publishAttribute("$settable", this->settable ? "true" : "false");
      if (this->format != 0)
        this->publishAttribute("$format", this->format);
      if (this->unit != 0)
        this->publishAttribute("$unit", this->unit);

      if (this->value != nullptr)
        this->getMqttClient()->publish(this->mqttPath, *this->value, true);

      if (this->settable)
      {
        this->logger->debug("Subscribing to %s", this->setterTopic);
        this->getMqttClient()->subscribe(this->setterTopic, [this](const String &value)
                                         {
                                           this->logger->debug("Received set request on %s", this->setterTopic);
                                           this->callback(value);
                                         });
      }
    }

    void HomieProperty::setValue(String *value)
    {
      if (this->value != nullptr)
      {
        delete this->value;
      }
      this->value = value;
      this->getMqttClient()->publish(this->mqttPath, *value, true);
    }

    void HomieProperty::setValue(float value, uint8_t precision)
    {
      this->setValue(new String(value, precision));
    }

    void HomieProperty::setValue(double value, uint8_t precision)
    {
      this->setValue(new String(value, precision));
    }

    void HomieProperty::setValue(int value)
    {
      this->setValue(new String(value));
    }

    void HomieProperty::setValue(bool value)
    {
      this->setValue(new String(value ? "true" : "false"));
    }

    void HomieProperty::setValue(char *value)
    {
      this->setValue(new String(value));
    }

    void HomieProperty::setValue(const char *value)
    {
      this->setValue(new String(value));
    }

    const char *HomieProperty::getValue()
    {
      return this->value->c_str();
    }

    void HomieProperty::publishAttribute(const char *name, String value, bool retain)
    {
      char topic[strlen(name) + strlen(this->mqttPath) + 2];
      strcpy(topic, this->mqttPath);
      strcat(topic, "/");
      strcat(topic, name);
      this->getMqttClient()->publish(topic, value, retain);
    }

    void HomieProperty::setSettable(MessageReceivedCallback callback)
    {
      this->settable = true;
      this->callback = callback;
    }

    EspMQTTClient *HomieProperty::getMqttClient()
    {
      return this->node->getMqttClient();
    }

    void HomieProperty::setUnit(const char *value)
    {
      this->unit = value;
    }

    const char *HomieProperty::getUnit()
    {
      return this->unit;
    }

    void HomieProperty::setFormat(const char *value)
    {
      this->format = value;
    }

    const char *HomieProperty::getFormat()
    {
      return this->format;
    }
  }
}