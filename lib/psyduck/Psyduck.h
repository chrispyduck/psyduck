#ifndef PSYDUCK_LOCALWIFI_H
#define PSYDUCK_LOCALWIFI_H

#if defined(ARDUINO_ESP8266_RELEASE)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <EspMQTTClient.h>
#include "base/psyduck-base.h"
#include "homie/psyduck-homie.h"
#include "sensor-interfaces/psyduck-sensor-interfaces.h"
#include "gpio/psyduck-gpio.h"

#define STATS_INTERVAL 30

using namespace psyduck::homie;
using namespace psyduck::sensors;
using psyduck::base::ConnectionStatusLight;
using psyduck::gpio::GpioPinId;

namespace psyduck
{
  struct PsyduckConfigWifi
  {
    char *ssid = nullptr;
    char *password = nullptr;
  };

  struct PsyduckConfigMqtt
  {
    char *server = nullptr;
    char *username = nullptr;
    char *password = nullptr;
    char *clientId = nullptr;
    ushort port = 9001;
  };

  struct PsyduckConfig
  {
    PsyduckConfigWifi wifi;
    PsyduckConfigMqtt mqtt;
    char *deviceId = nullptr;
    char *deviceName = nullptr;
    const char *otaPassword = nullptr;
    uint16_t otaPort = 0;
  };

  class Psyduck
  {
  public:
    Psyduck(PsyduckConfig config);

    bool isWifiConnected();
    bool isMqttConnected();

    EspMQTTClient *getEspMQTTClient();

    // To be called during the process' main loop
    void loop();

    HomieDevice *getHomieDevice() { return this->homieDevice; }

    void activateFaultState();
    void onConnectionEstablished();

    void setConnectionStatusLight(GpioPinId ledPin);
    ConnectionStatusLight* getConnectionStatusLight();

    void setDebug(bool enabled);

  private:
    ConnectionStatusLight *connectionStatusLight = nullptr;
    Logger *logger = nullptr;
    EspMQTTClient *mqttClient = nullptr;
    HomieDevice *homieDevice = nullptr;
    PsyduckConfig config;
    bool isFaulted = false;

    Timers::Task statsTimer;
    HomieNode *stats = nullptr;
    HomieProperty *wifiChannelProperty = nullptr;
    HomieProperty *uptimeProperty = nullptr;
    HomieProperty *rssiProperty = nullptr;
    HomieProperty *connectionEstablishedCountProperty = nullptr;
    HomieProperty *timerCountProperty = nullptr;
    HomieProperty *currentFreeHeap = nullptr;
    HomieProperty *minFreeHeap = nullptr;
    HomieProperty *lastResetReason = nullptr;
    
    HomieProperty *chipInfo = nullptr;

    static bool publishStatsTimerTick(void *);
    void publishStats();
  };
}

#endif