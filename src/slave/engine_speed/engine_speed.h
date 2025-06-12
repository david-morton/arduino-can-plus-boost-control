#pragma once

#include <Arduino.h>
#include <ptScheduler.h>

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

int  calculateRpm();
int  getCurrentRpm();
void updateRpmPulse();
void updateRpmSchedulerFrequency(ptScheduler &scheduler, int currentRpm);
