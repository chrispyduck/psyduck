#ifndef HOMIE_H
#define HOMIE_H

#include <Arduino.h>
#include <LinkedList.h>
#include <EspMQTTClient.h>
#include "../base/psyduck-base.h"

#define STATS_INTERVAL 30
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
      void publishStats();

      LinkedList<HomieNode *> getNodes();
      char *getName();
      char *getMqttPath();

      void registerNode(HomieNode *node);
      EspMQTTClient *getMqttClient();

    private:
      const char *id;
      const char *name;
      char *mqttPath;
      EspMQTTClient *client;
      LinkedList<HomieNode *> nodes;
      Timers::Task statsTimer;

      Logger *logger;

      void publishAttribute(const char *name, String value, bool retain = true);

      static bool publishStatsTimerTick(void*);
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
      const char *id;
      const char *name;
      const char *type;
      char *mqttPath;
      LinkedList<HomieProperty *> properties;
      HomieDevice *device;

      Logger *logger;

      void publishAttribute(const char *name, String value, bool retain = true);
    };

    class HomieProperty
    {
    public:
      HomieProperty(HomieNode *node, const char *id, const char *name, const char *datatype);
      void publish();

      const char *getName();
      const char *getId();

      void setValue(String* value);
      void setValue(int value);
      void setValue(float value, uint8_t precision = 1);
      const char* getValue();

      void setUnit(const char *value);
      const char *getUnit();

      void setFormat(const char *value);
      const char *getFormat();

      void setSettable(MessageReceivedCallback callback);

    private:
      const char *id;
      const char *name;
      const char *type;
      const char *format = NULL;
      const char *unit = NULL;
      bool settable = false;
      char *mqttPath;
      HomieNode *node;
      String *value = NULL;
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
