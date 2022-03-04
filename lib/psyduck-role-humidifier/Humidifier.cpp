
#include "Humidifier.h"

using psyduck::base::Logger;
using psyduck::sensors::IHumidity;
using namespace psyduck::homie;

namespace psyduck
{
  namespace roles
  {
    namespace humidifier
    {
      Humidifier::Humidifier(
          psyduck::Psyduck *main,
          ITemperatureAndHumidity *sensor,
          HumidifierHardwareConfiguration hardwareConfiguration)
      {
        this->logger = new Logger(__FILE__);
        this->sensor = sensor;
        this->hardwareConfiguration = hardwareConfiguration;

        this->settings.load();

        this->configNode = new HomieNode(main->getHomieDevice(), "humidifierConfig", "Humidifier Configuration", "humidifierConfig");

        pinMode(hardwareConfiguration.atomizerRelayPin, OUTPUT);
        pinMode(hardwareConfiguration.waterLevelLowSensorPin, INPUT_PULLDOWN);
        pinMode(hardwareConfiguration.waterValveOpenPin, OUTPUT);
        ledcAttachPin(hardwareConfiguration.fanPwmPin, 1);
        ledcSetup(1, 16000, 8);

        this->configDesiredHumidityProperty = HomieProperty::percentage(
            this->configNode,
            "desiredHumidity",
            "Desired Humidity",
            this->settings.getDesiredHumidity(),
            [this](const String &value)
            {
              auto val = this->settings.setDesiredHumidity(value.toFloat());
              this->configDesiredHumidityProperty->setValue(val);
            });

        this->configLowHumidityProperty = HomieProperty::percentage(
            this->configNode,
            "lowHumidity",
            "Low Humidity Level",
            this->settings.getLowHumidity(),
            [this](const String &value)
            {
              auto val = this->settings.setLowHumidity(value.toFloat());
              this->configLowHumidityProperty->setValue(val);
            });

        this->configAtomizerMaxDutyCycleProperty = HomieProperty::percentage(
            this->configNode,
            "atomizerMaxDutyCycle",
            "Atomizer Maximum Duty Cycle",
            this->settings.getAtomizerMaxDutyCycle() * 100,
            [this](const String &value)
            {
              auto val = this->settings.setAtomizerMaxDutyCycle(value.toFloat() / 100);
              this->configAtomizerMaxDutyCycleProperty->setValue(val);
            });

        this->configAtomizerMinDutyCycleProperty = HomieProperty::percentage(
            this->configNode,
            "atomizerMinDutyCycle",
            "Atomizer Minimum Duty Cycle",
            this->settings.getAtomizerMinDutyCycle() * 100,
            [this](const String &value)
            {
              auto val = this->settings.setAtomizerMinDutyCycle(value.toFloat() / 100);
              this->configAtomizerMinDutyCycleProperty->setValue(val);
            });

        this->configFanHighDutyCycleProperty = HomieProperty::percentage(
            this->configNode,
            "fanHighDutyCycle",
            "Fan Duty Cycle (High Speed)",
            this->settings.getFanHighDutyCycle() * 100,
            [this](const String &value)
            {
              auto val = this->settings.setFanHighDutyCycle(value.toFloat() / 100);
              this->configFanHighDutyCycleProperty->setValue(val);
            });

        this->configFanIdleDutyCycleProperty = HomieProperty::percentage(
            this->configNode,
            "fanLowDutyCycle",
            "Fan Duty Cycle (Low/Idle Speed)",
            this->settings.getFanIdleDutyCycle() * 100,
            [this](const String &value)
            {
              auto val = this->settings.setFanIdleDutyCycle(value.toFloat() / 100);
              this->configFanIdleDutyCycleProperty->setValue(val);
            });

        this->stateNode = new HomieNode(main->getHomieDevice(), "humidifierState", "Humidifier State", "humidifierState");

        this->currentFanDutyCycleProperty = HomieProperty::percentage(
            this->stateNode,
            "fanDutyCycle",
            "Current Fan Duty Cycle",
            0);

        this->currentAtomizerPowerStateProperty = HomieProperty::boolean(
            this->stateNode,
            "atomizerOn",
            "Atomizer On",
            false);

        this->currentAtomizerDutyCycleProperty = HomieProperty::percentage(
            this->stateNode,
            "atomizerDutyCycle",
            "Current Atomizer Duty Cycle",
            0);

        this->currentWaterValveStatusProperty = HomieProperty::boolean(
            this->stateNode,
            "waterValveOpen",
            "Water Valve Open",
            false);

        this->currentWaterLevelLowProperty = HomieProperty::boolean(
            this->stateNode,
            "waterLevelLow",
            "Water Level Low",
            true);

        Timers::every(1000, Humidifier::timerTick, this);
      }

      bool Humidifier::timerTick(void *ref)
      {
        Humidifier *instance = static_cast<Humidifier *>(ref);
        instance->check();
        return true;
      }

      void Humidifier::check()
      {
        this->checkWaterValve();
        this->checkAtomizer();
        this->checkFanSpeed();
      }

      void Humidifier::checkWaterValve()
      {
        bool isOpen = this->gpioState.waterValveOpen;
        bool shouldBeOpen = this->shouldWaterValveBeOpen();
        if (isOpen == shouldBeOpen)
          return;
        else if (isOpen)
        {
          digitalWrite(this->gpioState.waterValveOpen, LOW);
          this->currentWaterValveStatusProperty->setValue(false);
        }
        else if (shouldBeOpen)
        {
          digitalWrite(this->gpioState.waterValveOpen, HIGH);
          this->currentWaterValveStatusProperty->setValue(true);
        }
      }

      void Humidifier::checkFanSpeed()
      {
        bool fanHigh = this->gpioState.fanIsHigh;
        bool fanShouldBeHigh = this->gpioState.atomizerRelayActive;
        if (!fanShouldBeHigh)
        {
          // run fan for at least 5 minutes after last use of atomizer
          fanShouldBeHigh = (millis() - lastAtomizerOff < 300000);
        }

        if (fanHigh == fanShouldBeHigh)
          return;
        else if (fanHigh)
        {
          this->gpioState.fanIsHigh = false;
          this->gpioState.fanDutyCycle = this->settings.getFanIdleDutyCycle() * 255;
          this->currentFanDutyCycleProperty->setValue(this->gpioState.fanDutyCycle / 255.0f);
          this->logger->info("Setting fan duty cycle to %x", this->gpioState.fanDutyCycle);
          ledcWrite(1, this->gpioState.fanDutyCycle);
        }
        else if (fanShouldBeHigh)
        {
          this->gpioState.fanIsHigh = true;
          this->gpioState.fanDutyCycle = this->settings.getFanHighDutyCycle() * 255;
          this->currentFanDutyCycleProperty->setValue(this->gpioState.fanDutyCycle / 255.0f);
          this->logger->info("Setting fan duty cycle to %x", this->gpioState.fanDutyCycle);
          ledcWrite(1, this->gpioState.fanDutyCycle);
        }
      }

      void Humidifier::checkAtomizer()
      {
        auto atomizerOn = this->gpioState.atomizerRelayActive;
        auto now = millis();
        auto dutyCycle = this->computeAtomizerDutyCycle();

        // convert duty cycle to seconds on and off
        auto secondsOn = dutyCycle * 60;
        auto secondsOff = 60 - secondsOn;

        if (atomizerOn)
        {
          // atomizer is on; see if we need to turn it off
          if ((now - this->lastAtomizerOn) / 1000 > secondsOn)
          {
            // we do
            this->logger->info("Deactivating atomizer for %is", secondsOff);
            this->lastAtomizerOff = now;
            digitalWrite(this->hardwareConfiguration.atomizerRelayPin, LOW);
            this->currentAtomizerPowerStateProperty->setValue(true);
          }
        }
        else
        {
          // atomizer is off; see if we need to turn it on
          if ((now - this->lastAtomizerOff) / 1000 > secondsOff)
          {
            // we do!
            this->logger->info("Activating atomizer for %is", secondsOn);
            this->lastAtomizerOn = now;
            digitalWrite(this->hardwareConfiguration.atomizerRelayPin, HIGH);
            this->currentAtomizerPowerStateProperty->setValue(false);
          }
        }
      }

      bool Humidifier::shouldWaterValveBeOpen()
      {
        bool previousValue = this->gpioState.waterLevelLowSensor;
        bool newValue = digitalRead(this->hardwareConfiguration.waterLevelLowSensorPin) == HIGH;

        if (previousValue != newValue)
        {
          this->gpioState.waterLevelLowSensor = newValue;
          this->currentWaterLevelLowProperty->setValue(this->gpioState.waterLevelLowSensor);
        }

        auto humidity = this->sensor->getHumidity();
        if (humidity >= this->settings.getDesiredHumidity())
          return false;

        return this->gpioState.waterLevelLowSensor;
      }

      float Humidifier::computeAtomizerDutyCycle()
      {
        // sanity check humidity value
        auto currentHumidity = this->sensor->getHumidity();
        if (currentHumidity >= 100 || currentHumidity <= 0)
        {
          this->logger->warn("Read invalid humidity value of %f; ignoring and disabling atomizer", currentHumidity);
          return 0;
        }

        // 0.0 - 1.0 factor of where the humidity is; higher means closer to desired value
        auto setPoint = this->settings.getDesiredHumidity();
        auto lowPoint = this->settings.getLowHumidity();
        float humidityFactor;
        if (currentHumidity > setPoint)
        {
          humidityFactor = 1;
        }
        else if (currentHumidity < lowPoint)
        {
          humidityFactor = 0;
        }
        else
        {
          humidityFactor = (currentHumidity - lowPoint) / (setPoint - lowPoint);
        }

        // invert and multiply humidity factor by max duty cycle
        auto atomizerMax = this->settings.getAtomizerMaxDutyCycle();
        auto atomizerMin = this->settings.getAtomizerMinDutyCycle();
        auto atomizerDutyCycle = ((1 - humidityFactor) * (atomizerMax - atomizerMin)) + atomizerMin;
        this->logger->debug("Computed atomizer duty cycle of %.2f (hFactor=%.2f, RH=%f)",
                            atomizerDutyCycle,
                            humidityFactor,
                            currentHumidity);
        this->currentAtomizerDutyCycleProperty->setValue(atomizerDutyCycle * 100);
        return atomizerDutyCycle;
      }
    }
  }
}