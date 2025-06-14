#pragma once

#include <Arduino.h>
#include <ptScheduler.h>

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

int  calculateRpm();
int  getCurrentEngineSpeedRpm();
void updateRpmPulse();
void updateRpmSchedulerFrequency(ptScheduler &scheduler, int currentEngineSpeedRpm);
