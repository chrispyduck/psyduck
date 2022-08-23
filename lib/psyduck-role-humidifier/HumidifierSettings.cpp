#include "HumidifierSettings.h"

namespace psyduck
{
  namespace roles
  {
    namespace humidifier
    {
      HumidifierSettings::HumidifierSettings()
      {
        this->init("humidifier");
      }

      float HumidifierSettings::getDesiredHumidity()
      {
        return this->data->desiredHumidity;
      }

      float HumidifierSettings::setDesiredHumidity(float value)
      {
        if (value < this->data->lowHumidity)
          value = this->data->lowHumidity;
        else if (value > 90)
          value = 90;

        this->data->desiredHumidity = value;
        this->save();
        return value;
      }

      float HumidifierSettings::getLowHumidity()
      {
        return this->data->lowHumidity;
      }

      float HumidifierSettings::setLowHumidity(float value)
      {
        if (this->data->lowHumidity > 50)
          value = 50;
        else if (this->data->lowHumidity < 0)
          value = 0;
        this->data->lowHumidity = value;
        this->save();
        return value;
      }

      float HumidifierSettings::getFanIdleDutyCycle()
      {
        return this->data->fanIdleDutyCycle;
      }

      float HumidifierSettings::setFanIdleDutyCycle(float value)
      {
        if (value < 0)
          value = 0;
        else if (value > 1)
          value = 1;

        this->data->fanIdleDutyCycle = value;
        this->save();
        return value;
      }

      float HumidifierSettings::getFanHighDutyCycle()
      {
        return this->data->fanHighDutyCycle;
      }

      float HumidifierSettings::setFanHighDutyCycle(float value)
      {
        if (value < 0)
          value = 0;
        else if (value > 1)
          value = 1;

        this->data->fanHighDutyCycle = value;
        this->save();
        return value;
      }

      float HumidifierSettings::getAtomizerMinDutyCycle()
      {
        return this->data->atomizerMinDutyCycle;
      }

      float HumidifierSettings::setAtomizerMinDutyCycle(float value)
      {
        if (value < 0)
          value = 0;
        else if (value > 1)
          value = 1;

        this->data->atomizerMinDutyCycle = value;
        this->save();
        return value;
      }

      float HumidifierSettings::getAtomizerMaxDutyCycle()
      {
        return this->data->atomizerMaxDutyCycle;
      }

      float HumidifierSettings::setAtomizerMaxDutyCycle(float value)
      {
        if (value < 0)
          value = 0;
        else if (value > 1)
          value = 1;

        this->data->atomizerMaxDutyCycle = value;
        this->save();
        return value;
      }
    }
  }
}