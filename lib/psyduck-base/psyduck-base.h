#ifndef _PSYDUCK_BASE_H
#define _PSYDUCK_BASE_H

#include "Settings.h"
#include "LocalWifi.h"
#include "serial.h"
#include "Logger.h"
#include "Timer.h"

namespace psyduck
{
  namespace base
  {
    void setup()
    {
      psyduck::base::serial::setup();
    }
  }
}

#endif