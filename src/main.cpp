#include <Arduino.h>
#include <Psyduck.h>
#include <psyduck-sensor-aht20.h>
#include <psyduck-fans.h>

#define PIN_PWM 13
#define PIN_TACH1 27
#define PIN_TACH2 33
#define COMPUTE_INTERVAL 5000

psyduck::Psyduck *main;
psyduck::PsyduckConfig config;
psyduck::sensors::AHT20 *sensor;
psyduck::fans::Tachometer *fan1;
psyduck::fans::Tachometer *fan2;
psyduck::fans::SpeedControllerConfiguration speedControllerConfig;
psyduck::fans::SpeedController *controller;
psyduck::base::Logger *logger;
long int lastProcess = 0;

void IRAM_ATTR tach1()
{
  fan1->readPulse();
}
void IRAM_ATTR tach2()
{
  fan2->readPulse();
}

void compute()
{
  sensor->read();
  controller->computeDutyCycle();
  fan1->computeRpm();
  fan2->computeRpm();
  lastProcess = millis();
}

void setup()
{
  logger = new psyduck::base::Logger(__FILE__);

  config.deviceId = "media-rack-fan";
  config.deviceName = "Media Rack Fan";
  config.wifi.ssid = "thing2";
  config.wifi.password = "";
  config.mqtt.server = "";
  config.mqtt.username = "";
  config.mqtt.password = "";
  config.mqtt.port = 9001;

  main = new psyduck::Psyduck(config);

  sensor = new psyduck::sensors::AHT20(main);

  speedControllerConfig.pwmGpioPin = PIN_PWM;
  controller = new psyduck::fans::SpeedController(main, speedControllerConfig, sensor);

  fan1 = new psyduck::fans::Tachometer(controller->getHomieNode(), "fan1speed", "Fan 1 Speed", PIN_TACH1, tach1);
  fan2 = new psyduck::fans::Tachometer(controller->getHomieNode(), "fan2speed", "Fan 2 Speed", PIN_TACH2, tach2);

  compute();
}

void onConnectionEstablished()
{
  main->onConnectionEstablished();
}

void loop()
{
  main->getEspMQTTClient()->loop();
  Timers::tick();

  if (millis() - lastProcess > COMPUTE_INTERVAL)
  {
    compute();
  }
}