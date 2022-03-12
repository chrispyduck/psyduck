
#include "Humidifier.h"

using psyduck::base::Logger;
using psyduck::sensors::IHumidity;
using namespace psyduck::homie;

namespace
{
  const short FAN_PWM_CHANNEL = 1;
  const short ATOMIZER_PWM_CHANNEL = 1;
}

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

        pinMode(hardwareConfiguration.waterLevelLowSensorPin, INPUT_PULLDOWN);

        pinMode(hardwareConfiguration.waterValveOpenPin, OUTPUT);
        digitalWrite(hardwareConfiguration.waterValveOpenPin, LOW);

        this->gpioState.atomizer.init(hardwareConfiguration.atomizerPwmPin);
        this->gpioState.fan.init(hardwareConfiguration.fanPwmPin);
        this->gpioState.waterLevelLowSensor = false;
        this->gpioState.waterValveOpen = false;

        this->configNode = new HomieNode(main->getHomieDevice(), "humidifierConfig", "Humidifier Configuration", "humidifierConfig");

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
              this->configAtomizerMaxDutyCycleProperty->setValue(val * 100);
            });

        this->configAtomizerMinDutyCycleProperty = HomieProperty::percentage(
            this->configNode,
            "atomizerMinDutyCycle",
            "Atomizer Minimum Duty Cycle",
            this->settings.getAtomizerMinDutyCycle() * 100,
            [this](const String &value)
            {
              auto val = this->settings.setAtomizerMinDutyCycle(value.toFloat() / 100);
              this->configAtomizerMinDutyCycleProperty->setValue(val * 100);
            });

        this->configFanHighDutyCycleProperty = HomieProperty::percentage(
            this->configNode,
            "fanHighDutyCycle",
            "Fan Duty Cycle (High Speed)",
            this->settings.getFanHighDutyCycle() * 100,
            [this](const String &value)
            {
              auto val = this->settings.setFanHighDutyCycle(value.toFloat() / 100);
              this->configFanHighDutyCycleProperty->setValue(val * 100);
            });

        this->configFanIdleDutyCycleProperty = HomieProperty::percentage(
            this->configNode,
            "fanLowDutyCycle",
            "Fan Duty Cycle (Low/Idle Speed)",
            this->settings.getFanIdleDutyCycle() * 100,
            [this](const String &value)
            {
              auto val = this->settings.setFanIdleDutyCycle(value.toFloat() / 100);
              this->configFanIdleDutyCycleProperty->setValue(val * 100);
            });

        this->stateNode = new HomieNode(main->getHomieDevice(), "humidifierState", "Humidifier State", "humidifierState");

        this->currentFanDutyCycleProperty = HomieProperty::percentage(
            this->stateNode,
            "fanDutyCycle",
            "Current Fan Duty Cycle",
            0);

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

        Timers::every(this->hardwareConfiguration.checkInterval, Humidifier::timerTick, this);
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
          this->logger->info("Closing water valve");
          digitalWrite(this->hardwareConfiguration.waterValveOpenPin, LOW);
          this->currentWaterValveStatusProperty->setValue(false);
          this->gpioState.waterValveOpen = false;
        }
        else if (shouldBeOpen)
        {
          this->logger->info("Opening water valve");
          digitalWrite(this->hardwareConfiguration.waterValveOpenPin, HIGH);
          this->currentWaterValveStatusProperty->setValue(true);
          this->gpioState.waterValveOpen = true;
        }
      }

      void Humidifier::checkFanSpeed()
      {
        // ideal fan speed matches atomizer speed
        float atomizerDutyCycle = this->gpioState.atomizer.getFloat();

        // shift fan speed to agree with configured range
        // don't turn fan off until 5 minutes after atomizer off
        float max = this->settings.getFanHighDutyCycle();
        float min = this->settings.getFanIdleDutyCycle();
        float newFanDutyCycle = (atomizerDutyCycle == 0 && (millis() - this->lastAtomizerActive) > 300000)
                                    ? 0
                                    : ((max - min) * atomizerDutyCycle) + min;
        float previousFanDutyCycle = this->gpioState.fan.getFloat();
        if (abs(previousFanDutyCycle - newFanDutyCycle) > 0.005)
        {
          this->logger->debug("Computed fan duty cycle of %.3f (%+.3f)",
                              newFanDutyCycle,
                              newFanDutyCycle - previousFanDutyCycle);
          this->gpioState.fan.set(newFanDutyCycle);
          this->currentFanDutyCycleProperty->setValue(newFanDutyCycle * 100);
        }
      }

      void Humidifier::checkAtomizer()
      {
        auto previousDutyCycle = this->gpioState.atomizer.getFloat();
        auto newDutyCycle = this->computeAtomizerDutyCycle();
        if (abs(previousDutyCycle - newDutyCycle) > 0.005)
        {
          this->logger->debug("Computed atomizer duty cycle of %.3f (RH=%.1f, %+.3f)",
                              newDutyCycle,
                              this->sensor->getHumidity(),
                              newDutyCycle - previousDutyCycle);
          this->gpioState.atomizer.set(newDutyCycle);
          this->currentAtomizerDutyCycleProperty->setValue(newDutyCycle * 100);
        }
        if (newDutyCycle > 0)
          this->lastAtomizerActive = millis();
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
        return atomizerDutyCycle;
      }
    }
  }
}