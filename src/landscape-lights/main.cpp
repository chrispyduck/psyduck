#include <Psyduck.h>
#include <psyduck-role-humidifier.h>
#include <psyduck-role-switchArray.h>

#include "wifi-settings.secret.h"
#include "landscape-lights.secret.h"
#include "ota.secret.h"

#define PIN_CH1 19
#define PIN_CH2 21
#define PIN_CH3 22
#define PIN_CH4 23
#define PIN_LED 13

psyduck::Psyduck *psyduckMain = nullptr;
psyduck::PsyduckConfig config;
psyduck::base::Logger *logger = nullptr;
psyduck::base::ConnectionStatusLight *statusLight = nullptr;
psyduck::roles::switcharray::SwitchArray *switchArray = nullptr;
psyduck::roles::switcharray::SwitchArrayHardwareConfiguration hwConfig;

void setup()
{
  logger = new psyduck::base::Logger(__FILE__);

  config.deviceId = const_cast<char*>(DEVICE_ID);
  config.deviceName = const_cast<char*>(DEVICE_NAME);
  config.otaPassword = OTA_PASSWORD;
  config.otaPort = OTA_PORT;
  config.wifi.ssid = const_cast<char*>(WIFI_SSID);
  config.wifi.password = const_cast<char*>(WIFI_PASSWORD);
  config.mqtt.server = const_cast<char*>(MQTT_SERVER);
  config.mqtt.username = const_cast<char*>(MQTT_USERNAME);
  config.mqtt.password = const_cast<char*>(MQTT_PASSWORD);
  config.mqtt.port = MQTT_PORT;

  psyduckMain = new psyduck::Psyduck(config);
  psyduckMain->setDebug(true);

  hwConfig.channel1 = PIN_CH1;
  hwConfig.channel2 = PIN_CH2;
  hwConfig.channel3 = PIN_CH3;
  hwConfig.channel4 = PIN_CH4;
  switchArray = new psyduck::roles::switcharray::SwitchArray(hwConfig);
  switchArray->init(psyduckMain);

  psyduckMain->setConnectionStatusLight(PIN_LED);
}

void loop()
{
  psyduckMain->loop();
}