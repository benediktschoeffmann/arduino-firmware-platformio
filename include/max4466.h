#include <Arduino.h>
#include "settings.h"

#ifndef _MAX_4466_H
#define _MAX_4466_H

float readMax4466() {
    float min = (2 << ADC_WIDTH_BIT_12) - 1;
    float max = 0;
    int sample = analogRead(PIN_MAX_4466);
}


#endif