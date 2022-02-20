#include "Settings.h"
#include <EEPROM.h>

namespace
{
  // global variables internal to settings
  int global_data_offset = EEPROM_DATA_OFFSET;
}

namespace psyduck
{
  namespace base
  {

    void Settings::commit()
    {
      EEPROM.commit();
    }

    template <typename TData>
    SettingsInstance<TData>::SettingsInstance()
    {
      this->offset = global_data_offset;
      global_data_offset += sizeof(TData);

      this->data = new TData();
    }

    template <typename TData>
    void SettingsInstance<TData>::load()
    {
      EEPROM.get(EEPROM_DATA_OFFSET, *this->data);
    }

    template <typename TData>
    void SettingsInstance<TData>::save()
    {
      EEPROM.put(EEPROM_DATA_OFFSET, *this->data);
    }

  }
}
