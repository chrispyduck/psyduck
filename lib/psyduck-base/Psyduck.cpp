#include "Psyduck.h"

using namespace psyduck::homie;

namespace psyduck
{
  namespace base
  {
    Psyduck::Psyduck(PsyduckConfig config) {
      psyduck::base::serial::setup();

      // prepare configuration
      this->config = config;
      if (config.mqtt.clientId == NULL) {
        config.mqtt.clientId = config.deviceId;
      }

      // create client
      this->mqttClient = new EspMQTTClient(
        config.wifi.ssid,
        config.wifi.password,
        config.mqtt.server,
        config.mqtt.username,
        config.mqtt.password,
        config.mqtt.clientId,
        config.mqtt.port
      );

      this->homieDevice = new HomieDevice(
        this->mqttClient,
        config.deviceId,
        config.deviceName
      );
    }

    bool Psyduck::isWifiConnected()
    {
      return this->mqttClient->isWifiConnected();
    }

    bool Psyduck::isMqttConnected()
    {
      return this->mqttClient->isMqttConnected();
    }

    EspMQTTClient* Psyduck::getEspMQTTClient()
    {
      return this->mqttClient;
    }

    void Psyduck::loop() {
      this->mqttClient->loop();
      Timers::tick();
    }
  }
}