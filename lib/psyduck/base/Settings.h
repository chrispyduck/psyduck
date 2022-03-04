#ifndef SETTINGS_H
#define SETTINGS_H

#include <EEPROM.h>
#include "Logger.h"

#define EEPROM_DATA_OFFSET 0
#define EEPROM_HEADER "OK v1"

namespace psyduck
{
  namespace base
  {
    template <typename TData>
    class SettingsInstance;

    class Settings
    {
    private: 
      static int global_data_offset;

    public:
      static void commit();

      template <typename TData> friend class SettingsInstance;
    };

    template <typename TData>
    class SettingsInstance : Settings
    {
    private:
      int offset = 0;
      Logger *logger = nullptr;

    protected:
      TData *data = nullptr;

    public:
      SettingsInstance()
      {
        this->logger = new Logger(__FILE__);

        this->offset = global_data_offset;
        global_data_offset += sizeof(TData);

        this->data = new TData();

        this->logger->debug(
          "Creating settings instance at offset %i (new global offset=%i, data size=%i)", 
          this->offset,
          global_data_offset,
          sizeof(TData));
      }

      // loads the persisted settings into the `data` member
      void load()
      {
        this->logger->debug("Loading data from offset %i", this->offset);
        EEPROM.get(this->offset, *this->data);
      }

      // schedules a write of the current value of the `data` member to persistent storage.
      // the write is deferred until `Settings::commit()` is called.
      void save()
      {
        this->logger->debug("Saving data to offset %i", this->offset);
        EEPROM.put(this->offset, *this->data);
      }
    };
  }
}

#endif