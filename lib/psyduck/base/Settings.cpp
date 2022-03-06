#include "Settings.h"
#include "Logger.h"
#include "Timer.h"

namespace
{
  psyduck::base::Logger logger(__FILE__);
  psyduck::base::Timers::Task commitHoldTimer = 0;
}

namespace psyduck
{
  namespace base
  {
    void Settings::commit()
    {
      if (commitHoldTimer != 0) {
        Timers::cancel(commitHoldTimer);
      }
      logger.debug("Committing all settings to EEPROM");
      EEPROM.commit();
    }

    void Settings::markDirty()
    {
      if (commitHoldTimer != 0) {
        Timers::cancel(commitHoldTimer);
      }
      commitHoldTimer = Timers::in(3000, autoCommitHoldTimerElapsed);
    }

    bool Settings::autoCommitHoldTimerElapsed(void*) {
      Settings::commit();
      return false;
    }

    int Settings::global_data_offset = EEPROM_DATA_OFFSET;
  }
}
