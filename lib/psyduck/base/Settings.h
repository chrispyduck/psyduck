#ifndef SETTINGS_H
#define SETTINGS_H

#define EEPROM_DATA_OFFSET 0
#define EEPROM_HEADER "OK v1"

namespace psyduck
{
  namespace base
  {

    class Settings
    {
    public:
      static void commit();
    };

    template <typename TData>
    class SettingsInstance : Settings
    {
    private:
      int offset = 0;

    protected:
      TData *data;

    public:
      SettingsInstance();
      
      // loads the persisted settings into the `data` member
      void load();

      // schedules a write of the current value of the `data` member to persistent storage.
      // the write is deferred until `Settings::commit()` is called.
      void save();
    };

  }
}

#endif