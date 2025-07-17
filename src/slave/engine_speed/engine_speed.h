#pragma once

#include <Arduino.h>
#include <ptScheduler.h>

/* ======================================================================
   CONSTANTS
   ====================================================================== */

const int rpmCalculationFrequencyThreshold = 1500; // RPM threshold to switch between fast and slow update frequency
const int rpmPulsesPerRevolution           = 3;    // Number of pulses on the signal wire per crank revolution

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

int  calculateRpm();
int  getCurrentEngineSpeedRpm();
void updateRpmPulse();
void updateRpmSchedulerFrequency(ptScheduler &scheduler, int currentEngineSpeedRpm);
