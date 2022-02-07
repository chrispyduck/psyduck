#ifndef HOMIENODE_H
#define HOMIENODE_H

#include "HomieDevice.h"
#include "HomieProperty.h"
#include <LinkedList.h>
#include <psyduck-base.h>

using psyduck::base::Logger;

namespace psyduck
{
  namespace homie
  {
    class HomieProperty;

    class HomieNode
    {
    public:
      HomieNode(HomieDevice *device, const char *id, const char *name, const char *type);
      void publish();

      LinkedList<HomieProperty *> getProperties();
      const char *getName();
      const char *getId();
      char *getMqttPath();

      void registerProperty(HomieProperty *property);
      EspMQTTClient *getMqttClient();

    private:
      const char *id;
      const char *name;
      const char *type;
      char *mqttPath;
      LinkedList<HomieProperty *> properties;
      HomieDevice *device;

      Logger *logger;

      void publishAttribute(const char *name, String value, bool retain = true);
    };

  }
}

#endif