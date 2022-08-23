#ifndef PSYDUCK_CONNECTIONSTATUSLIGHT_H
#define PSYDUCK_CONNECTIONSTATUSLIGHT_H

#include "StatusLight.h"
#include <functional>

namespace psyduck
{
  namespace base
  {
    enum ConnectionStatus
    {
      STATUS_FAULT = 0,
      STATUS_WIFI_DISCONNECTED = 1,
      STATUS_MQTT_DISCONNECTED = 2,
      STATUS_ONLINE = 3
    };

    class ConnectionStatusLight : private StatusLight
    {
    public:
      ConnectionStatusLight(int gpioPin,
                            std::function<bool()> getWifiConnected,
                            std::function<bool()> getMqttConnected)
          : StatusLight(gpioPin)
      {
        this->getWifiConnected = getWifiConnected;
        this->getMqttConnected = getMqttConnected;
        this->setBlinkInterval(INTERVAL_WIFI_DISCONNECTED);
      };

      void activateFaultIndicator();
      void pulse();

    private:
      const int INTERVAL_WIFI_DISCONNECTED = 1500;
      const int INTERVAL_MQTT_DISCONNECTED = 500;
      const int INTERVAL_FAULT = 3000;

      std::function<bool()> getWifiConnected;
      std::function<bool()> getMqttConnected;

      void timerTick() override;
      ConnectionStatus lastStatus = STATUS_WIFI_DISCONNECTED;
    };
  }
}

#endif