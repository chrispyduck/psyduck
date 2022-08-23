#include "homie-declarations.h"

using psyduck::base::Logger;

namespace psyduck
{
  namespace homie
  {
    HomieNode::HomieNode(HomieDevice *device, const char *id, const char *name, const char *type)
    {
      this->id = id;
      this->name = name;
      this->type = type;
      this->device = device;

      this->mqttPath = new char[strlen(id) + strlen(device->getMqttPath()) + 3];
      strcpy(this->mqttPath, device->getMqttPath());
      strcat(this->mqttPath, this->id);
      strcat(this->mqttPath, "/");

      this->logger = new Logger(this->mqttPath);

      device->registerNode(this);
    }

    void HomieNode::registerProperty(HomieProperty *property)
    {
      this->properties.add(property);
    }

    void HomieNode::publish()
    {
      this->logger->debug("publishing");

      this->publishAttribute("$name", this->name);
      this->publishAttribute("$type", this->type);

      HomieProperty *property;
      char propertyNames[256];
      propertyNames[0] = '\0';
      bool first = true;
      for (int i = 0; i < this->properties.size(); i++)
      {
        property = this->properties.get(i);
        property->publish();

        if (first)
          first = false;
        else
          strcat(propertyNames, ",");
        strcat(propertyNames, property->getId());
      }
      this->publishAttribute("$properties", propertyNames);
    }

    void HomieNode::publishAttribute(const char *name, String value, bool retain)
    {
      char topic[strlen(name) + strlen(this->mqttPath) + 1];
      strcpy(topic, this->mqttPath);
      strcat(topic, name);
      this->getMqttClient()->publish(topic, value, retain);
    }

    const char *HomieNode::getId()
    {
      return this->id;
    }

    const char *HomieNode::getName()
    {
      return this->name;
    }

    LinkedList<HomieProperty *> HomieNode::getProperties()
    {
      return this->properties;
    }

    char *HomieNode::getMqttPath()
    {
      return this->mqttPath;
    }

    EspMQTTClient *HomieNode::getMqttClient()
    {
      return this->device->getMqttClient();
    }
  }
}