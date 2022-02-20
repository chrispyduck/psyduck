#ifndef BSEC_SETTINGS_H
#define BSEC_SETTINGS_H

#include <Psyduck.h>
#include <bsec.h>

using psyduck::base::SettingsInstance;

namespace psyduck
{
  namespace sensors
  {
    namespace bsec
    {
      struct BsecData
      {
        BsecData();

        byte bsecState[BSEC_MAX_STATE_BLOB_SIZE];
      };

      class BsecSettings : public SettingsInstance<BsecData>
      {
      public:
        BsecSettings();
        uint8_t *getBsecState();

      private:
        BsecData data;
      };
    }
  }
}

#endif