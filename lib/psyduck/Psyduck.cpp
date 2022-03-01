#include "Psyduck.h"

using namespace psyduck::homie;

namespace psyduck
{
  Psyduck::Psyduck(PsyduckConfig config)
  {
    Serial.begin(115200);
    while (!Serial)
      ;
    
    this->logger = new Logger(__FILE__);
    this->logger->info(F("Starting..."));

    // prepare configuration
    this->config = config;
    if (config.mqtt.clientId == nullptr)
    {
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
        config.mqtt.port);
    this->mqttClient->enableDebuggingMessages(true);
    this->mqttClient->setMaxPacketSize(256);

    this->homieDevice = new HomieDevice(
        this->mqttClient,
        config.deviceId,
        config.deviceName);

    this->logger->debug(F("Startup complete"));
  }

  bool Psyduck::isWifiConnected()
  {
    return this->mqttClient->isWifiConnected();
  }

  bool Psyduck::isMqttConnected()
  {
    return this->mqttClient->isMqttConnected();
  }

  EspMQTTClient *Psyduck::getEspMQTTClient()
  {
    return this->mqttClient;
  }

  void Psyduck::onConnectionEstablished()
  {
    this->logger->info(F("Connection established. Publishing homie messages."));
    this->homieDevice->publish();
    this->homieDevice->setReady();
    this->homieDevice->publishStats();
    this->logger->debug(F("Done publishing homie messages"));
  }

  inline void Psyduck::loop()
  {
    this->mqttClient->loop();
    Timers::tick();
  }
}