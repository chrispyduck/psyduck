#ifndef PSYDUCK_LOCALWIFI_H
#define PSYDUCK_LOCALWIFI_H

#if defined(ARDUINO_ESP8266_RELEASE)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <EspMQTTClient.h>
#include "./base/psyduck-base.h"
#include "./homie/psyduck-homie.h"
#include "./sensor-interfaces/psyduck-sensor-interfaces.h"
#include "./gpio/GPIO.h"

using namespace psyduck::homie;
using namespace psyduck::sensors;

namespace psyduck
{
  struct PsyduckConfigWifi
  {
    char *ssid;
    char *password;
  };

  struct PsyduckConfigMqtt
  {
    char *server;
    char *username;
    char *password;
    char *clientId;
    ushort port = 9001;
  };

  struct PsyduckConfig
  {
    PsyduckConfigWifi wifi;
    PsyduckConfigMqtt mqtt;
    char *deviceId;
    char *deviceName;
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

  private:
    Logger *logger;
    EspMQTTClient *mqttClient;
    HomieDevice *homieDevice;
    PsyduckConfig config;
  };
}

#endif