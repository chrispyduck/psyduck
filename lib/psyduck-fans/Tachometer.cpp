#include "Tachometer.h"
#include <Psyduck.h>

namespace psyduck
{
  namespace fans
  {
    Tachometer::Tachometer(HomieNode* parentNode, const char* id, const char* name, short tachPin, ISR isr)
    {
      this->logger = new Logger(__FILE__);
      this->tachPin = tachPin;
      this->rpmLastReadMillis = millis();
      this->revolutions = 0;

      this->rpmProperty = new HomieProperty(parentNode, id, name, "integer");
      this->rpmProperty->setFormat("0:20000");
      this->rpmProperty->setUnit("RPM");

      pinMode(tachPin, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(tachPin), isr, RISING);
      sei();

      Timers::every(5000, timerTick, this);
    }

    bool Tachometer::timerTick(void *ref)
    {
      Tachometer *instance = static_cast<Tachometer *>(ref);
      instance->computeRpm();
      return true;
    }

    IRAM_ATTR void Tachometer::readPulse()
    {
      this->revolutions++;
    }

    int computeRpmFromRevs(int revs, float factor)
    {
      // simplification of: revs / 2 / (factor) * 60
      return revs / 2 / factor * 60;
      //return 30 * revs * factor;
    }

    void Tachometer::computeRpm()
    {
      long int now = millis();
      long int mark = now - this->rpmLastReadMillis;
      if (mark > 1000)
      {
        int rpm = this->revolutions;
        this->revolutions = 0;
        this->rpmLastReadMillis = now;
        float factor = mark / 1000.0;
        this->rpm = computeRpmFromRevs(rpm, factor);
        this->logger->debug("Fan at pin %d RPM = %d", this->tachPin, this->rpm);
        this->rpmProperty->setValue(this->rpm);
      }
    }
  }
}