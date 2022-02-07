#include "BsecSettings.h"

using namespace psyduck::sensors::bsec;

namespace psyduck
{
  namespace sensors
  {
    namespace bsec
    {
      BsecData::BsecData()
      {
        for (int i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++)
        {
          this->data[i] = 0;
        }
      }

      BsecSettings::BsecSettings() {}

      uint8_t *BsecSettings::getBsecState()
      {
        return this->data.bsecState;
      }
    }
  }
}