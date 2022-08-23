#ifndef HOMIE_H
#define HOMIE_H

#include <Arduino.h>
#include <LinkedList.h>
#include <EspMQTTClient.h>
#include "../base/psyduck-base.h"

#define HOMIE_VERSION "3.0.0"
#define HOMIE_PREFIX "homie"

using namespace psyduck::base;

namespace psyduck
{
  namespace homie
  {
    enum HomieDeviceStatus
    {
      HOMIE_DEVICE_INIT,
      HOMIE_DEVICE_READY,
      HOMIE_DEVICE_DISCONNECTED,
      HOMIE_DEVICE_SLEEPING,
      HOMIE_DEVICE_LOST,
      HOMIE_DEVICE_ALERT
    };

    class HomieNode;
    class HomieProperty;

    class HomieDevice
    {
    public:
      HomieDevice(EspMQTTClient *client, const char *id, const char *name);
      void publish();
      void setStatus(HomieDeviceStatus status);
      void setReady();
      void handleDisconnect();

      LinkedList<HomieNode *> getNodes();
      char *getName();
      char *getMqttPath();

      void registerNode(HomieNode *node);
      EspMQTTClient *getMqttClient();

    private:
      const char *id = nullptr;
      const char *name = nullptr;
      char *mqttPath = nullptr;
      EspMQTTClient *client = nullptr;
      LinkedList<HomieNode *> nodes;

      Logger *logger = nullptr;

      void publishAttribute(const char *name, String value, bool retain = true);
    };

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
      const char *id = nullptr;
      const char *name = nullptr;
      const char *type = nullptr;
      char *mqttPath = nullptr;
      LinkedList<HomieProperty *> properties;
      HomieDevice *device = nullptr;

      Logger *logger = nullptr;

      void publishAttribute(const char *name, String value, bool retain = true);
    };

    class HomieProperty
    {
    public:
      HomieProperty(HomieNode *node, const char *id, const char *name, const char *datatype);
      void publish();

      const char *getName();
      const char *getId();

      void setValue(String *value);
      void setValue(int value);
      void setValue(unsigned int value);
      void setValue(long value);
      void setValue(long unsigned int value);
      void setValue(float value, uint8_t precision = 1);
      void setValue(double value, uint8_t precision = 1);
      void setValue(char *value);
      void setValue(const char *value);
      void setValue(bool value);

      void setUnit(const char *value);
      const char *getUnit();

      void setFormat(const char *value);
      const char *getFormat();

      void setSettable(MessageReceivedCallback callback);

      static HomieProperty *percentage(HomieNode *node, const char *id, const char *name, float currentValue = 0, MessageReceivedCallback setter = nullptr);
      static HomieProperty *minutes(HomieNode *node, const char *id, const char *name, float currentValue = 0, MessageReceivedCallback setter = nullptr);
      static HomieProperty *boolean(HomieNode *node, const char *id, const char *name, bool currentValue = false, MessageReceivedCallback setter = nullptr);
      static HomieProperty *counter(HomieNode *node, const char *id, const char *name, int currentValue = 0);

      static HomieProperty *build(
          HomieNode *node,
          const char *id,
          const char *name,
          const char *type,
          const char *unit = nullptr,
          const char *format = nullptr,
          MessageReceivedCallback setter = nullptr);

    private:
      const char *id = nullptr;
      const char *name = nullptr;
      const char *type = nullptr;
      const char *format = nullptr;
      const char *unit = nullptr;
      bool settable = false;
      char *mqttPath = nullptr;
      HomieNode *node = nullptr;
      String *value = nullptr;
      MessageReceivedCallback callback;
      char *setterTopic = nullptr;

      Logger *logger = nullptr;

    protected:
      void publishAttribute(const char *name, String value, bool retain = true);
      EspMQTTClient *getMqttClient();
    };

  }
}
#endif
