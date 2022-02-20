#include "Thermostat.h"

using psyduck::base::Logger;
using psyduck::sensors::ITemperatureAndHumidity;
using namespace psyduck::homie;

namespace psyduck
{
  namespace roles
  {
    namespace thermostat
    {
      Thermostat::Thermostat(
        psyduck::Psyduck* main, 
        ITemperatureAndHumidity *sensor,
        ThermostatHardwareConfiguration hardwareConfiguration)
      {
        this->logger = new Logger(__FILE__);
        this->sensor = sensor;
        this->hardwareConfiguration = hardwareConfiguration;

        this->thermostatNode = new HomieNode(main->getHomieDevice(), "thermostat", "Thermostat", "thermostat");

        this->fanOnProperty = new HomieProperty(this->thermostatNode, "fanOn", "Fan On", "boolean");
        this->fanOnProperty->setSettable(
            [this](const String &value)
            {
              if (value == "true")
              {
                this->settings.setFanOn(true);
              }
              else if (value == "false")
              {
                this->settings.setFanOn(false);
              }
              this->checkThermostat();

              // acknowledge and report
              this->fanOnProperty->setValue(settings.getFanOn() ? "true" : "false");
            });
        this->fanOnProperty->setValue(settings.getFanOn() ? "true" : "false");

        this->heatOnProperty = new HomieProperty(this->thermostatNode, "heatOn", "Heat On", "boolean");

        this->setPointProperty = new HomieProperty(this->thermostatNode, "setPoint", "Set Point", "float");
        this->setPointProperty->setUnit("°F");
        this->setPointProperty->setFormat("45:90");
        this->setPointProperty->setSettable(
            [this](const String &value)
            {
              float val = value.toFloat();
              val = this->settings.setSetPoint(val);
              this->setPointProperty->setValue(String(val));
              this->checkThermostat();
            });
        this->setPointProperty->setValue(String(settings.getSetPoint()));

        this->fanDurationAfterHeatProperty = new HomieProperty(this->thermostatNode, "fanDurationAfterHeat", "Fan Duration After Heat Off", "float");
        this->fanDurationAfterHeatProperty->setUnit("Minutes");
        this->fanDurationAfterHeatProperty->setFormat("0:60");
        this->fanDurationAfterHeatProperty->setSettable(
            [this](const String &value)
            {
              float floatValue = value.toFloat();
              floatValue = this->settings.setFanDurationAfterHeat(floatValue);
              this->fanDurationAfterHeatProperty->setValue(String(floatValue));
            });
        this->fanDurationAfterHeatProperty->setValue(String(settings.getFanDurationAfterHeat()));

        this->temperatureMarginProperty = new HomieProperty(this->thermostatNode, "temperatureMargin", "Temperature Margin", "float");
        this->temperatureMarginProperty->setUnit("°F");
        this->temperatureMarginProperty->setFormat("0:10");
        this->temperatureMarginProperty->setSettable(
            [this](const String &value)
            {
              float val = value.toFloat();
              val = this->settings.setTemperatureMargin(val);
              this->temperatureMarginProperty->setValue(String(val));
            });
        this->temperatureMarginProperty->setValue(String(settings.getTemperatureMargin()));

        this->safetyMinimumTemperatureProperty = new HomieProperty(this->thermostatNode, "safetyMinimumTemperature", "Safety Minimum Temperature", "float");
        this->safetyMinimumTemperatureProperty->setUnit("°F");
        this->safetyMinimumTemperatureProperty->setFormat("40:90");
        this->safetyMinimumTemperatureProperty->setSettable(
            [this](const String &value)
            {
              float val = value.toFloat();
              val = this->settings.setSafetyMinimumTemperature(val);
              this->safetyMinimumTemperatureProperty->setValue(String(val));
            });
        this->safetyMinimumTemperatureProperty->setValue(String(settings.getSafetyMinimumTemperature()));

        this->safetyMaximumTemperatureProperty = new HomieProperty(this->thermostatNode, "safetyMaximumTemperature", "Safety Maximum Temperature", "float");
        this->safetyMaximumTemperatureProperty->setUnit("°F");
        this->safetyMaximumTemperatureProperty->setFormat("40:90");
        this->safetyMaximumTemperatureProperty->setSettable(
            [this](const String &value)
            {
              float val = value.toFloat();
              val = this->settings.setSafetyMaximumTemperature(val);
              this->safetyMaximumTemperatureProperty->setValue(String(val));
            });
        this->safetyMaximumTemperatureProperty->setValue(String(settings.getSafetyMaximumTemperature()));
      }

      void Thermostat::checkThermostat()
      {
        this->logger->debug("verifying environmental settings");

        auto setPoint = this->settings.getSetPoint();

        float currentTemp = this->sensor->getTemperature();
        bool desiredHeatSetting = false;
        bool desiredFanSetting = false;
        if (currentTemp > setPoint + this->settings.getTemperatureMargin())
        {
          desiredHeatSetting = false;
          desiredFanSetting = false;
        }
        else if (currentTemp < setPoint - this->settings.getTemperatureMargin())
        {
          desiredHeatSetting = true;
          desiredFanSetting = true;
        }

        if (desiredHeatSetting != this->gpioState.heatOn || this->firstRun)
        {
          if (!desiredHeatSetting && this->gpioState.heatOn)
            this->lastHeatOff = millis();
          this->setHeatOn(desiredHeatSetting);
          this->heatOnProperty->setValue(desiredHeatSetting ? "true" : "false");
        }

        if (settings.getFanOn())
          desiredFanSetting = true;
        else if (millis() - this->lastHeatOff < this->settings.getFanDurationAfterHeat() * 60000)
          desiredFanSetting = true; // leave fan on for 5 minutes after turning off heat
        if (desiredFanSetting != this->gpioState.fanOn || this->firstRun)
        {
          this->setFanOn(desiredFanSetting);
          this->fanOnProperty->setValue(desiredFanSetting ? "true" : "false");
        }

        this->logger->info("done: fan=%T, heat=%T, setPoint=%F, fanAlwaysOn=%T",
                     desiredFanSetting,
                     desiredHeatSetting,
                     setPoint,
                     this->settings.getFanOn());
        this->firstRun = false;
      }

      void Thermostat::setFanOn(bool desiredState) {
        psyduck::gpio::GPIO::setSwitch(this->hardwareConfiguration.fanOn, desiredState);
      }

      void Thermostat::setHeatOn(bool desiredState) {
        psyduck::gpio::GPIO::setSwitch(this->hardwareConfiguration.heatOn, desiredState);
      }
    }
  }
}