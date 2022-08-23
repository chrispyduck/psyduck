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
    this->mqttClient->enableOTA(config.otaPassword, config.otaPort);

    this->homieDevice = new HomieDevice(
        this->mqttClient,
        config.deviceId,
        config.deviceName);

    this->stats = new HomieNode(this->homieDevice, "stats", "stats", "stats");
    this->wifiChannelProperty = HomieProperty::counter(this->stats, "wifiChannel", "Channel");
    this->uptimeProperty = HomieProperty::counter(this->stats, "uptime", "Uptime", 0);
    this->rssiProperty = HomieProperty::build(this->stats, "rssi", "RSSI", "integer", "dBm");
    this->connectionEstablishedCountProperty = HomieProperty::counter(this->stats, "connectionEstablishedCount", "Connection Established Count", 0);
    this->timerCountProperty = HomieProperty::counter(this->stats, "timerCount", "Active Timers", 0);
    this->chipInfo = HomieProperty::build(this->stats, "chipInfo", "Chip Information", "info");
    this->minFreeHeap = HomieProperty::counter(this->stats, "minFreeHeap", "Free Heap (Minimum)");
    this->currentFreeHeap = HomieProperty::counter(this->stats, "currentFreeHeap", "Free Heap (Current)");
    this->lastResetReason = HomieProperty::build(this->stats, "lastResetReason", "Last Reset Reason", "enum");

    this->statsTimer = Timers::every(STATS_INTERVAL * 1000, &publishStatsTimerTick, this);
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
    this->publishStats();
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

  void Psyduck::setConnectionStatusLight(GpioPinId ledPin)
  {
    this->connectionStatusLight = new ConnectionStatusLight(
        ledPin,
        std::bind(&EspMQTTClient::isWifiConnected, this->mqttClient),
        std::bind(&EspMQTTClient::isMqttConnected, this->mqttClient));
  }

  ConnectionStatusLight *Psyduck::getConnectionStatusLight()
  {
    return this->connectionStatusLight;
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

  bool Psyduck::publishStatsTimerTick(void *mainVoid)
  {
    Psyduck *main = static_cast<Psyduck *>(mainVoid);
    main->publishStats();
    return true;
  }

  void Psyduck::publishStats()
  {
    this->wifiChannelProperty->setValue(WiFi.channel());
    this->rssiProperty->setValue(WiFi.RSSI());

    this->uptimeProperty->setValue(millis());
    this->connectionEstablishedCountProperty->setValue(this->mqttClient->getConnectionEstablishedCount());
    this->timerCountProperty->setValue(Timers::size());

    esp_chip_info_t info;
    esp_chip_info(&info);
    char infoStr[128];
    sprintf(infoStr, "model=%u, features=%u, cores=%u, revision=%u", info.model, info.features, info.cores, info.revision);
    this->chipInfo->setValue(infoStr);

    this->minFreeHeap->setValue(esp_get_minimum_free_heap_size());
    this->currentFreeHeap->setValue(esp_get_free_heap_size());
    this->lastResetReason->setValue(esp_reset_reason());
  }
}