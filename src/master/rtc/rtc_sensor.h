#pragma once

#include <RTClib.h>

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void     initialiseRtc();
float    getRtcCurrentTemperature();
DateTime getRtcCurrentDateTime();
