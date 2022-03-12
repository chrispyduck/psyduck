#include "ThermostatSettings.h"

namespace psyduck
{
  namespace roles
  {
    namespace thermostat
    {
      ThermostatSettings::ThermostatSettings()
      {
        this->init("thermostat");
      }

      float ThermostatSettings::getSetPoint()
      {
        return this->data->setPoint;
      }

      float ThermostatSettings::setSetPoint(float value)
      {
        if (value < this->data->safetyMinimumTemperature)
          value = this->data->safetyMinimumTemperature;
        else if (value > this->data->safetyMaximumTemperature)
          value = this->data->safetyMaximumTemperature;

        this->data->setPoint = value;
        this->save();
        return value;
      }

      bool ThermostatSettings::getFanOn()
      {
        return this->data->fanOn;
      }

      bool ThermostatSettings::setFanOn(bool value)
      {
        this->data->fanOn = value;
        this->save();
        return value;
      }

      float ThermostatSettings::getFanDurationAfterHeat()
      {
        return this->data->fanDurationAfterHeat;
      }

      float ThermostatSettings::setFanDurationAfterHeat(float minutes)
      {
        if (minutes < 0)
          minutes = 0;

        this->data->fanDurationAfterHeat = minutes;
        this->save();
        return minutes;
      }

      float ThermostatSettings::getSafetyMinimumTemperature()
      {
        return this->data->safetyMinimumTemperature;
      }

      float ThermostatSettings::setSafetyMinimumTemperature(float temperature)
      {
        if (temperature < 40)
          temperature = 40;
        else if (temperature > 90)
          temperature = 90;
        this->data->safetyMinimumTemperature = temperature;
        this->save();
        return temperature;
      }

      float ThermostatSettings::getSafetyMaximumTemperature()
      {
        return this->data->safetyMaximumTemperature;
      }

      float ThermostatSettings::setSafetyMaximumTemperature(float temperature)
      {
        if (temperature < 40)
          temperature = 40;
        else if (temperature > 90)
          temperature = 90;
        this->data->safetyMaximumTemperature = temperature;
        this->save();
        return temperature;
      }

      float ThermostatSettings::getTemperatureMargin()
      {
        return this->data->temperatureMargin;
      }

      float ThermostatSettings::setTemperatureMargin(float margin)
      {
        if (margin < 0)
          margin = 0;
        else if (margin > 10)
          margin = 10;

        this->data->temperatureMargin = margin;
        this->save();
        return margin;
      }
    }
  }
}