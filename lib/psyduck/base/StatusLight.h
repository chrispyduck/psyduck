#ifndef PSYDUCK_WIFISTATUSLIGHT_H
#define PSYDUCK_WIFISTATUSLIGHT_H

#include "Timer.h"

namespace psyduck {
  namespace base {

    const unsigned long PULSE_INTERVAL = 10; 

    enum StatusLightBehavior {
      STATUS_LIGHT_BEHAVIOR_STEADY = 0,
      STATUS_LIGHT_BEHAVIOR_BLINK = 1
    };

    class StatusLight {
      public:
        StatusLight(int gpioPin);

        void setBlinkInterval(unsigned long intervalMilliseconds);
        void setAlwaysOn();
        void setAlwaysOff();
        void pulse();

        friend bool statusTimerTick(void*);

      protected:
        virtual void timerTick();

      private:
        bool lightOn;
        int interval;
        int pin;
        StatusLightBehavior behavior;
        Timers::Task timer;
    };
  }
}

#endif