#pragma once
#include "NodeWrapper.h"

namespace psyduck
{
  namespace homie
  {
    class DeviceStats : public NodeWrapper
    {
    public:
      DeviceStats(HomieDevice *device, EspMQTTClient *client) : NodeWrapper(device, "stats", "Stats", "stats")
      {
        this->client = client;
      };

    private:
      void init();

      EspMQTTClient *client;
      HomieProperty *uptime;
      HomieProperty *rssi;
      HomieProperty *connectionEstablishedCount;
      HomieProperty *timerCount;

      void publish();
      static bool doInstancePublish(void*);
    };
  }
}