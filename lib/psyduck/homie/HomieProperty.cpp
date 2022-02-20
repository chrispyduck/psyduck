#include "homie-declarations.h"

using psyduck::base::Logger;

namespace psyduck
{
  namespace homie
  {

    HomieProperty::HomieProperty(HomieNode *node, const char *id, const char *name, const char *datatype)
    {
      this->logger = new Logger((std::string("HomieProperty:") + id).c_str());

      this->id = id;
      this->name = name;
      this->type = datatype;
      this->node = node;

      this->mqttPath = new char[strlen(id) + strlen(node->getMqttPath()) + 3];
      strcpy(this->mqttPath, node->getMqttPath());
      strcat(this->mqttPath, this->id);

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
      this->publishAttribute("$name", this->name);
      this->publishAttribute("$datatype", this->type);
      this->publishAttribute("$settable", this->settable ? "true" : "false");
      if (this->format != 0)
        this->publishAttribute("$format", this->format);
      if (this->unit != 0)
        this->publishAttribute("$unit", this->unit);

      if (this->value != NULL)
        this->getMqttClient()->publish(this->mqttPath, *this->value, true);

      if (this->settable)
      {
        if (this->setterTopic == 0)
        {
          this->setterTopic = new char[strlen(this->mqttPath) + 5];
          strcpy(this->setterTopic, this->mqttPath);
          strcat(this->setterTopic, "/set");
        }
        this->getMqttClient()->subscribe(this->setterTopic, [this](const String &value)
                                         {
                                           this->logger->debug("Received set request on %s", this->setterTopic);
                                           this->callback(value);
                                         });
      }
    }

    void HomieProperty::setValue(String* value)
    {
      if (this->value != NULL) {
        delete this->value;
      }
      this->value = value;
      this->getMqttClient()->publish(this->mqttPath, *value, true);
    }

    void HomieProperty::setValue(float value, uint8_t precision)
    {
      this->setValue(new String(value, precision));
      /*
      this->logger->debug("value=%d, precision=%d", value, precision);
      char buf[15];
      dtostrf(value, (precision + 2), precision, buf);
      this->logger->debug("buf=%s", buf);
      auto val = new String(buf);
      this->logger->debug("val=%d, *val=%d", val, *val);
      this->setValue(val);*/
    }

    void HomieProperty::setValue(int value)
    {
      this->setValue(new String(value));
    }

    const char* HomieProperty::getValue() {
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