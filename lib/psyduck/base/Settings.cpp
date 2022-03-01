#include "Settings.h"
#include "Logger.h"

namespace {
  psyduck::base::Logger logger(__FILE__);
}

namespace psyduck
{
  namespace base
  {
    void Settings::commit()
    {
      logger.debug("Committing all settings to EEPROM");
      EEPROM.commit();
    }

    int Settings::global_data_offset = EEPROM_DATA_OFFSET;
  }
}
