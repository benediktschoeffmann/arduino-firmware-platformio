#include <Arduino.h>
#include "settings.h"

#ifndef _ERROR_H
#define _ERROR_H

void error()
{
  bool led = true;
  while (true)
  {
    digitalWrite(PIN_ERROR_LED, led);
    delay(500);
    led = !led;
  }
}

#endif