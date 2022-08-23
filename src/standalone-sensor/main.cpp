#include <Psyduck.h>
#include <psyduck-role-humidifier.h>
#include <psyduck-sensor-aht20.h>

#include "wifi-settings.secret.h"
#include "parents-bath.secret.h"
#include "ota.secret.h"

#define PIN_FAN_PWM 15
#define PIN_ATOMIZER_PWM 32
#define PIN_WATER_LEVEL_LOW 27
#define PIN_WATER_VALVE 33
#define COMPUTE_INTERVAL 1000
#define PIN_BME688_CS 21
#define PIN_LED 13

psyduck::Psyduck *psyduckMain = nullptr;
psyduck::PsyduckConfig config;
psyduck::sensors::AHT20 *sensor = nullptr;
psyduck::base::Logger *logger = nullptr;
psyduck::base::ConnectionStatusLight *statusLight = nullptr;

bool timerTick(void *ref);

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
  logger->info("Initializing sensor");
  sensor = new psyduck::sensors::AHT20(psyduckMain);
  if (!sensor->isReady())
  {
    psyduckMain->activateFaultState();
    return;
  }
  sensor->read();

  psyduckMain->setConnectionStatusLight(PIN_LED);

  logger->trace("Starting timer");
  Timers::every(15000, timerTick);
}

bool timerTick(void *ref)
{
  sensor->read();
  return true;
}

void loop()
{
  psyduckMain->loop();
}