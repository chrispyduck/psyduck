#ifndef _CHRISPYDUCK_SERIAL_H
#define _CHRISPYDUCK_SERIAL_H

#ifndef SERIAL_SPEED
#define SERIAL_SPEED 115200
#endif

namespace psyduck
{
  namespace base
  {
    namespace serial
    {
      void setup()
      {
        Serial.begin(115200);
        while (!Serial)
          ;
      }
    }
  }
}

#endif