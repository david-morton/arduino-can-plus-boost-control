#pragma once

#include <RTClib.h>

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern int currentElectronicsRtcTemp;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void     initialiseRtc();
float    getRtcCurrentTemperature();
DateTime getRtcCurrentDateTime();
