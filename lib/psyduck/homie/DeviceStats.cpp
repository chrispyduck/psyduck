#include "homie-declarations.h"
#include "DeviceStats.h"
#include <functional>

namespace psyduck
{
  namespace homie
  {
    void DeviceStats::init()
    {
      this->uptime = HomieProperty::counter(this->getNode(), "uptime", "Uptime", millis() / 1000);

      this->rssi = HomieProperty::counter(this->getNode(), "rssi", "RSSI");
      this->rssi->setUnit("dBm");

      this->connectionEstablishedCount = HomieProperty::counter(this->getNode(), "connectionEstablishedCount", "Connection Count");

      this->timerCount = HomieProperty::counter(this->getNode(), "activeTimers", "Active Timers");

      Timers::every(30000, DeviceStats::doInstancePublish, this);
    }

    bool DeviceStats::doInstancePublish(void *ref)
    {
      DeviceStats* stats = static_cast<DeviceStats*>(ref);
      stats->publish();
      return true;
    }

    void DeviceStats::publish()
    {
      this->uptime->setValue(String(millis() / 1000));
      this->rssi->setValue(WiFi.RSSI());
      this->connectionEstablishedCount->setValue(String(this->client->getConnectionEstablishedCount()));
      this->timerCount->setValue(Timers::size());
    }
  }
}