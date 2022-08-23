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
    Preferences Settings::preferences;

    void Settings::commit()
    {
#ifdef SETTINGS_USE_EEPROM
      if (commitHoldTimer != 0)
      {
        Timers::cancel(commitHoldTimer);
      }
      logger.debug("Committing all settings to EEPROM");
      if (!EEPROM.commit())
      {
        logger.warn("Failed to commit settings to EEPROM");
      }
#endif
    }

    void Settings::markDirty()
    {
#ifdef SETTINGS_USE_EEPROM
      if (commitHoldTimer != 0)
      {
        Timers::cancel(commitHoldTimer);
      }
      commitHoldTimer = Timers::in(3000, autoCommitHoldTimerElapsed);
#endif
    }

    bool Settings::autoCommitHoldTimerElapsed(void *)
    {
      Settings::commit();
      return false;
    }

    int Settings::global_data_offset = EEPROM_DATA_OFFSET;
  }
}
