#ifndef HOMIE_H
#define HOMIE_H

#include <Arduino.h>
#include <LinkedList.h>
#include <EspMQTTClient.h>
#include <psyduck-base.h>

#define STATS_INTERVAL 30
#define HOMIE_VERSION "3.0.0"
#define HOMIE_PREFIX "homie"

using namespace psyduck::base;

namespace psyduck
{
  namespace homie
  {
    const char *HomieDeviceStatusStringMap[] = {
        "init",
        "ready",
        "disconnected",
        "sleeping",
        "lost",
        "alert"};

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

      //bool publishStatsTimerTick(void*);
    };

  }
}
#endif
