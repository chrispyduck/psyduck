#include "HomieDevice.h"
#include "HomieNode.h"
#include <psyduck-base.h>
#include <EspMQTTClient.h>

#define CONST_CONCAT(a, b) a b

using namespace psyduck::base;

namespace psyduck
{
  namespace homie
  {
    HomieDevice::HomieDevice(EspMQTTClient *client, const char *id, const char *name)
    {
      this->logger = new Logger((std::string("HomieDevice:") + id).c_str());
      this->id = id;
      this->name = name;
      this->client = client;

      this->mqttPath = new char[strlen(id) + strlen(HOMIE_PREFIX) + 3];
      strcpy(this->mqttPath, HOMIE_PREFIX);
      strcat(this->mqttPath, "/");
      strcat(this->mqttPath, id);
      strcat(this->mqttPath, "/");
    }

    void HomieDevice::publish()
    {
      this->setStatus(HOMIE_DEVICE_INIT);
      this->publishAttribute("$homie", HOMIE_VERSION);
      this->publishAttribute("$name", this->name);
      this->publishAttribute("$fw/name", this->name);
      this->publishAttribute("$fw/version", "0.1");
      this->publishAttribute("$implementation", "arduino+esp");
      this->publishAttribute("$stats", "uptime,rssi,connectionEstablishedCount");
      this->publishAttribute("$stats/interval", String(STATS_INTERVAL));

      char topic[strlen(name) + 7];
      strcpy(topic, this->mqttPath);
      strcat(topic, "$state");
      this->client->enableLastWillMessage(topic, "lost", true);

      HomieNode *node;
      char nodeNames[256];
      nodeNames[0] = '\0';
      bool first = true;
      for (int i = 0; i < this->nodes.size(); i++)
      {
        node = this->nodes.get(i);
        node->publish();

        if (first)
          first = false;
        else
          strcat(nodeNames, ",");
        strcat(nodeNames, node->getId());
      }
      this->publishAttribute("$nodes", nodeNames);
    }

    bool publishStatsTimerTick(void *deviceVoid)
    {
      HomieDevice *device = static_cast<HomieDevice *>(deviceVoid);
      device->publishStats();
      return true;
    }

    void HomieDevice::setReady()
    {
      this->setStatus(HOMIE_DEVICE_READY);
      this->statsTimer = Timers::every(STATS_INTERVAL * 1000, &publishStatsTimerTick, this);
    }

    void HomieDevice::handleDisconnect()
    {
      if (this->statsTimer != 0)
        Timers::cancel(this->statsTimer);
    }

    void HomieDevice::publishStats()
    {
      this->publishAttribute("$stats/uptime", String(millis()), false);
      this->publishAttribute("$stats/rssi", String(WiFi.RSSI()), false);
      this->publishAttribute("$stats/connectionEstablishedCount", String(this->client->getConnectionEstablishedCount()), false);
    }

    void HomieDevice::setStatus(HomieDeviceStatus status)
    {
      this->publishAttribute("$state", HomieDeviceStatusStringMap[status]);
    }

    void HomieDevice::registerNode(HomieNode *node)
    {
      this->nodes.add(node);
    }

    LinkedList<HomieNode *> HomieDevice::getNodes()
    {
      return this->nodes;
    }

    char *HomieDevice::getMqttPath()
    {
      return this->mqttPath;
    }

    EspMQTTClient *HomieDevice::getMqttClient()
    {
      return this->client;
    }

    void HomieDevice::publishAttribute(const char *name, String value, bool retain)
    {
      char topic[strlen(name) + strlen(this->mqttPath) + 1];
      strcpy(topic, this->mqttPath);
      strcat(topic, name);
      this->client->publish(topic, value, retain);
    }
  }
}