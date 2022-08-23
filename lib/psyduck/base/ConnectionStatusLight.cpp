#include "ConnectionStatusLight.h"
#include "Psyduck.h"

namespace psyduck
{
  namespace base
  {
    void ConnectionStatusLight::timerTick()
    {
      // check connection status and update if needed
      if (this->lastStatus != STATUS_FAULT)
      {
        ConnectionStatus currentStatus;
        if (!this->getWifiConnected())
        {
          currentStatus = STATUS_WIFI_DISCONNECTED;
        }
        else if (!this->getMqttConnected())
        {
          currentStatus = STATUS_MQTT_DISCONNECTED;
        }
        else
        {
          currentStatus = STATUS_ONLINE;
        }

        // status changed; update config
        if (this->lastStatus != currentStatus)
        {
          switch (currentStatus)
          {
          case STATUS_FAULT:
            this->setBlinkInterval(INTERVAL_FAULT);
            break;

          case STATUS_WIFI_DISCONNECTED:
            this->setBlinkInterval(INTERVAL_WIFI_DISCONNECTED);
            break;

          case STATUS_MQTT_DISCONNECTED:
            this->setBlinkInterval(INTERVAL_MQTT_DISCONNECTED);
            break;

          case STATUS_ONLINE:
            this->setAlwaysOn();
            break;
          }
          this->lastStatus = currentStatus;
        }
      }

      // call base method
      StatusLight::timerTick();
    }

    void ConnectionStatusLight::activateFaultIndicator()
    {
      this->lastStatus = STATUS_FAULT;
      StatusLight::timerTick();
    }

    void ConnectionStatusLight::pulse()
    {
      StatusLight::pulse();
    }
  }
}