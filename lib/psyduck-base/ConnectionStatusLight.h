#ifndef PSYDUCK_CONNECTIONSTATUSLIGHT_H
#define PSYDUCK_CONNECTIONSTATUSLIGHT_H

#include "StatusLight.h"

namespace psyduck {
  namespace base {
    enum ConnectionStatus {
      STATUS_UNKNOWN = 0,
      STATUS_WIFI_DISCONNECTED = 1,
      STATUS_MQTT_DISCONNECTED = 2,
      STATUS_ONLINE = 3
    };

    class ConnectionStatusLight : private StatusLight {
      public:
        ConnectionStatusLight(int gpioPin) : StatusLight(gpioPin) {
          this->setBlinkInterval(INTERVAL_WIFI_DISCONNECTED);
        };

      private:
        const int INTERVAL_WIFI_DISCONNECTED = 1500;
        const int INTERVAL_MQTT_DISCONNECTED = 500;

        void timerTick() override;
        ConnectionStatus lastStatus = STATUS_WIFI_DISCONNECTED;
    };
  }
}

#endif 