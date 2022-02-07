#ifndef HOMIEPROPERTY_H
#define HOMIEPROPERTY_H

#include "HomieNode.h"
#include <EspMQTTClient.h>
#include <psyduck-base.h>

using psyduck::base::Logger;

namespace psyduck
{
  namespace homie
  {
    class HomieProperty
    {
    public:
      HomieProperty(HomieNode *node, const char *id, const char *name, const char *datatype);
      void publish();

      const char *getName();
      const char *getId();

      void setValue(String value);
      void setValue(int value);
      void setValue(float value, uint8_t precision = 1);

      void setUnit(const char *value);
      const char *getUnit();

      void setFormat(const char *value);
      const char *getFormat();

      void setSettable(MessageReceivedCallback callback);

    private:
      const char *id;
      const char *name;
      const char *type;
      const char *format = 0;
      const char *unit = 0;
      bool settable = false;
      char *mqttPath;
      HomieNode *node;
      String value;
      MessageReceivedCallback callback;
      char *setterTopic;

      Logger *logger;

    protected:
      void publishAttribute(const char *name, String value, bool retain = true);
      EspMQTTClient *getMqttClient();
    };

  }
}
#endif
