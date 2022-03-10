#include "Psyduck.h"

using namespace psyduck::homie;
using psyduck::base::ConnectionStatusLight;

namespace
{
  psyduck::Psyduck *singleton = nullptr;

  void global_onConnectionEstablished()
  {
    singleton->onConnectionEstablished();
  }
}

void onConnectionEstablished()
{
  Serial.write("ERROR: global onConnectionEstablished called. this should not happen.");
  throw std::exception();
}

namespace psyduck
{
  Psyduck::Psyduck(PsyduckConfig config)
  {
    Serial.begin(115200);
    while (!Serial)
      ;

    this->logger = new Logger(__FILE__);
    if (singleton == nullptr)
    {
      singleton = this;
    }
    else
    {
      this->logger->info(F("Error: another instance of Psyduck already exists"));
      throw std::exception();
    }
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
    this->mqttClient->enableDebuggingMessages(false);
    this->mqttClient->setMaxPacketSize(512);
    this->mqttClient->setOnConnectionEstablishedCallback(global_onConnectionEstablished);

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

  void Psyduck::setDebug(bool enabled)
  {
    this->mqttClient->enableDebuggingMessages(enabled);
  }

  void Psyduck::onConnectionEstablished()
  {
    this->logger->info(F("Connection established. Publishing homie messages."));
    this->homieDevice->publish();
    this->homieDevice->setReady();
    this->homieDevice->publishStats();
    this->logger->debug(F("Done publishing homie messages"));
  }

  void Psyduck::loop()
  {
    if (this->isFaulted)
    {
      return;
    }

    this->mqttClient->loop();
    Timers::tick();
  }

  void Psyduck::setConnectionStatusLight(byte ledPin)
  {
    this->connectionStatusLight = new ConnectionStatusLight(
        ledPin,
        std::bind(&EspMQTTClient::isWifiConnected, this->mqttClient),
        std::bind(&EspMQTTClient::isMqttConnected, this->mqttClient));
  }

  void Psyduck::activateFaultState()
  {
    this->isFaulted = true;
    this->logger->error("Fault state activated. Processing halted. Restarting in 30 seconds.");
    if (this->connectionStatusLight != nullptr)
    {
      this->connectionStatusLight->activateFaultIndicator();
    }
    delay(30000);
    ESP.restart();
  }
}