#include "ConnectionStatusLight.h"
#include "Psyduck.h"

namespace psyduck {
  namespace base {
    void ConnectionStatusLight::timerTick() {
      ConnectionStatus currentStatus;
      // check connection status and update if needed
      if (!WiFi.isConnected()) {
        currentStatus = STATUS_WIFI_DISCONNECTED;
      }

      // status changed; update config
      if (this->lastStatus != currentStatus) {
        switch (currentStatus) {
          case STATUS_UNKNOWN:
            this->setAlwaysOff();
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

      // call base method 
      StatusLight::timerTick();
    }
  }
}