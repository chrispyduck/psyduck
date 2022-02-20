#ifndef PSYDUCK_FANS_TACH_H
#define PSYDUCK_FANS_TACH_H

#include <Arduino.h>
#include <Psyduck.h>

using psyduck::base::Logger;

namespace psyduck
{
  namespace fans
  {
    typedef void (*ISR)();

    class Tachometer
    {
    public:
      Tachometer(HomieNode* parentNode, const char* id, const char* name, short tachPin, ISR isr);

      void readPulse();
      int getRpm();

      void computeRpm();

    private:
      Logger* logger;
      short tachPin;
      int rpm;
      
      long int rpmLastReadMillis;
      int revolutions;

      HomieProperty* rpmProperty;

      
      static bool timerTick(void*);
    };
  }
}

#endif