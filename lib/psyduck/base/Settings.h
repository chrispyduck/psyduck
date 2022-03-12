#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef SETTINGS_USE_EEPROM
#include <EEPROM.h>
#else 
#include <Preferences.h>
#endif 

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
#ifndef SETTINGS_USE_EEPROM
      static Preferences preferences;
#endif

      static void markDirty();
      static bool autoCommitHoldTimerElapsed(void *);

    public:
      static void commit();

      template <typename TData>
      friend class SettingsInstance;
    };

    template <typename TData>
    class SettingsInstance : Settings
    {
    private:
      typedef uint8_t checksum_t;

      int offset = 0;
      Logger *logger = nullptr;
      checksum_t checksum = 0;
      bool initialized = false;
      const char* name;

    protected:
      TData *data = nullptr;

      void assertInitialized()
      {
        if (!this->initialized)
        {
          throw std::exception();
        }
      }

    public:
      SettingsInstance()
      {
      }

      void init(const char *name)
      {
        if (this->initialized)
        {
          return;
        }

        this->name = name;
        this->logger = new Logger("Settings", name);

        this->offset = global_data_offset;
        global_data_offset += sizeof(TData) + sizeof(checksum_t);
#ifdef SETTINGS_USE_EEPROM
        if (EEPROM.length() < global_data_offset) {
          this->logger->error("Insufficient EEPROM space available. Need %i bytes. Total length is %i.",
            global_data_offset,
            EEPROM.length()
          );
          throw std::exception();
        }
#endif

        this->data = new TData();

        this->logger->debug(
            "Creating settings instance at offset %i (new global offset=%i, data size=%i)",
            this->offset,
            global_data_offset,
            sizeof(TData));

        this->initialized = true;

        if (!this->load()) {
          this->save();
        }
      }

      // loads the persisted settings into the `data` member
      bool load()
      {
        this->assertInitialized();
        this->logger->debug("Loading %i bytes from offset %i", sizeof(TData), this->offset);
#ifdef SETTINGS_USE_EEPROM
        EEPROM.readBytes(this->offset, &this->checksum, sizeof(checksum_t));
        EEPROM.get(this->offset + sizeof(checksum_t), this->data);
#else 
        if (!Settings::preferences.begin(this->name, true)) {
          this->logger->warn("Failed to initialize preferences object");
          return false;
        }
        
        auto read = Settings::preferences.getBytes("checksum", &this->checksum, sizeof(checksum_t));
        if (read != sizeof(checksum_t)) {
          this->logger->warn("Failed to read checksum. Expected %i bytes, got %i.", sizeof(checksum_t), read);
          return false;
        }
        read = Settings::preferences.getBytes("data", this->data, sizeof(TData));
        if (read != sizeof(TData)) {
          this->logger->warn("Failed to read data. Expected %i bytes, got %i.", sizeof(checksum_t), read);
          return false;
        }
        Settings::preferences.end();
#endif
        checksum_t computedChecksum = this->computeChecksum();
        if (this->checksum != computedChecksum)
        {
          this->logger->warn("Data load failed: checksum invalid. Expected %X but was %X", this->checksum, computedChecksum);
          return false;
        }
        else
        {
          this->logger->trace("Successfully loaded data and validated checksum of %X", this->checksum);
          return true;
        }
      }

      // schedules a write of the current value of the `data` member to persistent storage.
      // the write is deferred until `Settings::commit()` is called.
      void save()
      {
        this->assertInitialized();
        this->checksum = this->computeChecksum();
        this->logger->debug("Saving %i bytes to offset %i (checksum: %X)", sizeof(TData), this->offset, this->checksum);
#ifdef SETTINGS_USE_EEPROM
        EEPROM.writeBytes(this->offset, &this->checksum, sizeof(checksum_t));
        EEPROM.put(this->offset + sizeof(checksum_t), this->data);
#else
        Settings::preferences.begin(this->name, false);
        Settings::preferences.putBytes("checksum", &this->checksum, sizeof(checksum_t));
        Settings::preferences.putBytes("data", this->data, sizeof(TData));
        Settings::preferences.end();
#endif
        Settings::markDirty();
      }

      checksum_t computeChecksum()
      {
        checksum_t checksum = 21;
        byte *dataRef = reinterpret_cast<byte *>(this->data);
        for (int i = 0; i < sizeof(TData); i++)
        {
          checksum += *dataRef++;
        }
        return checksum;
      }
    };
  }
}

#endif