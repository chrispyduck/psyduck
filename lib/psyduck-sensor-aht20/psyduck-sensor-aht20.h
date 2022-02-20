#ifndef _PSYDUCK_SENSOR_AHT20
#define _PSYDUCK_SENSOR_AHT20

#include <Psyduck.h>
#include <Adafruit_AHTX0.h>

namespace psyduck
{
  namespace sensors
  {
    class AHT20 : public ITemperatureAndHumidity
    {
      public:
        AHT20(Psyduck* psyduck);

        float getTemperature() override;
        float getHumidity() override;

        void read();

        friend bool timerTick(void*);

      private:
        Logger *logger;
        Psyduck* psyduck;
        Adafruit_AHTX0 aht;
        
        float temp;
        float hum;

        Adafruit_Sensor *aht_humidity, *aht_temp;

        HomieNode *node;
        HomieProperty *temperatureProperty;
        HomieProperty *humidityProperty;

        

        long int lastReport;
    };
  }
}

#endif