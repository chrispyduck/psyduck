#include <Arduino.h>
#include <psyduck-base.h>
#include <psyduck-homie.h>
#include <psyduck-role-thermostat.h>
#include <psyduck-sensor-bsec.h>

psyduck::base::Psyduck* main;
psyduck::base::PsyduckConfig config;
psyduck::roles::thermostat::Thermostat *thermostat;
psyduck::sensors::bsec::BsecSensor *sensor;

void setup() {
  config.deviceId = "humidifier";
  config.deviceName = "Humidifier";
  config.wifi.ssid = "thing2";
  config.wifi.password = "";
  config.mqtt.server = "";
  config.mqtt.username = "";
  config.mqtt.password = "";
  config.mqtt.port = 9001;

// config using arduinoJson?

  main = new psyduck::base::Psyduck(config);

  sensor = new psyduck::sensors::bsec::BsecSensor(main);

  thermostat = new psyduck::roles::thermostat::Thermostat(main, sensor);
}

void loop() {
  main->loop();

  
}