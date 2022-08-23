#include "StatusLight.h"
#include "Timer.h"
#include <Arduino.h>

namespace psyduck {
  namespace base {
    StatusLight::StatusLight(int gpioPin) {
      this->pin = gpioPin;
      this->behavior = STATUS_LIGHT_BEHAVIOR_STEADY;
      this->lightOn = false;

      pinMode(this->pin, OUTPUT);
      digitalWrite(this->pin, LOW);
    }

    bool statusTimerTick(void* ptr) {
      StatusLight* instance = static_cast<StatusLight*>(ptr);
      instance->timerTick();
      return true;
    }

    void StatusLight::setBlinkInterval(unsigned long intervalMilliseconds) {
      this->behavior = STATUS_LIGHT_BEHAVIOR_BLINK;
      this->interval = intervalMilliseconds;

      if (this->timer != 0) {
        Timers::cancel(this->timer);
        this->timer = 0;
      }
      
      this->timer = Timers::every(intervalMilliseconds, statusTimerTick, this);
    }

    void StatusLight::setAlwaysOn() {
      this->behavior = STATUS_LIGHT_BEHAVIOR_STEADY;
      this->lightOn = true;
      digitalWrite(this->pin, HIGH);

      if (this->timer != 0) {
        Timers::cancel(this->timer);
        this->timer = 0;
      }
    }

    void StatusLight::setAlwaysOff() {
      this->behavior = STATUS_LIGHT_BEHAVIOR_STEADY;
      this->lightOn = false;
      digitalWrite(this->pin, LOW);
      
      if (this->timer != 0) {
        Timers::cancel(this->timer);
        this->timer = 0;
      }
    }

    void StatusLight::pulse() {
      digitalWrite(this->pin, this->lightOn ? LOW : HIGH);
      delay(PULSE_INTERVAL);
      digitalWrite(this->pin, this->lightOn ? HIGH : LOW);
    }

    void StatusLight::timerTick() {
      bool newValue = !this->lightOn;
      if (newValue) {
        digitalWrite(this->pin, HIGH);
      } else {
        digitalWrite(this->pin, LOW);
      }
      this->lightOn = newValue;
    }
  }
}