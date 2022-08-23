#ifndef SWITCHARRAY_H
#define SWITCHARRAY_H

#include <Psyduck.h>

using psyduck::base::Logger;
using namespace psyduck::homie;
using namespace psyduck::gpio;

namespace psyduck
{
  namespace roles
  {
    namespace switcharray
    {
      struct SwitchArrayHardwareConfiguration
      {
        GpioPinId channel1;
        GpioPinId channel2;
        GpioPinId channel3;
        GpioPinId channel4;
      };

      class SwitchArray
      {

      public:
        SwitchArray(SwitchArrayHardwareConfiguration hardwareConfig);
        void init(psyduck::Psyduck *main);

      private:
        Logger *logger = nullptr;
        SwitchArrayHardwareConfiguration hardwareConfiguration;

        HomieNode *stateNode = nullptr;
        HomieProperty *channel1Property = nullptr;
        HomieProperty *channel2Property = nullptr;
        HomieProperty *channel3Property = nullptr;
        HomieProperty *channel4Property = nullptr;

        void setChannel(const GpioPinId &pin, const String &value, HomieProperty *property);
      };

    }

  }
}
#endif