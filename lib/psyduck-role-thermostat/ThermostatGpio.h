#include "StatusLight.h"
#include <Arduino.h>

namespace psyduck {
  namespace base {
    StatusLight::StatusLight(int gpioPin) {
      this->gpioPin = gpioPin;
      this->status = BAD;
      this->lightOn = false;

      pinMode(this->gpioPin, OUTPUT);
      digitalWrite(this->gpioPin, LOW);
    }

    void StatusLight::setStatus(STATUS status) {
      switch (status) {

      }
      this->status = status;
    }
  }
}