#include <Arduino.h>
#include <ptScheduler.h>

#include "../../shared/debug_logging.h"
#include "../pin_assignments_slave.h"
#include "engine_speed.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

const int              rpmCalculationFrequencyThreshold = 1500; // RPM threshold to switch between fast and slow update frequency
const int              rpmPulsesPerRevolution           = 3;    // Number of pulses on the signal wire per crank revolution
unsigned long          previousRpmPulseCounter;                 // Will store previous the number of pulses counted
unsigned long          previousRpmPulseTime;                    // Will store previous ISR micros value for calculations
volatile unsigned long latestRpmPulseCounter = 0;               // Will store latest number of pulses counted
volatile unsigned long latestRpmPulseTime    = micros();        // Will store latest ISR micros value for calculations

/* ======================================================================
   ISR: Update the RPM counter and time via interrupt
   ====================================================================== */

// This function is called every time a rising edge is detected on the RPM signal pin and updates the latest RPM pulse count and time.
void updateRpmPulse() {
  latestRpmPulseCounter++;
  latestRpmPulseTime = micros();
}

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Get the current RPM value
int getCurrentRpm() {
  if (latestRpmPulseCounter == 0) {
    DEBUG_ERROR("No RPM pulses detected yet, returning 0 RPM");
    return 0; // No pulses detected yet
  }

  detachInterrupt(digitalPinToInterrupt(ARDUINO_PIN_TACH_SIGNAL));
  int currentRpm = calculateRpm();
  attachInterrupt(digitalPinToInterrupt(ARDUINO_PIN_TACH_SIGNAL), updateRpmPulse, RISING);
  return currentRpm;
}

// Calculate RPM based on the latest pulse count and time
int calculateRpm() {
  unsigned long deltaMicros          = latestRpmPulseTime - previousRpmPulseTime;
  unsigned long deltaRpmPulseCounter = latestRpmPulseCounter - previousRpmPulseCounter;

  float microsPerPulse = deltaMicros / deltaRpmPulseCounter;

  float pulsesPerMinute = 60000000 / microsPerPulse;

  // Set previous variables to current values for next loop
  previousRpmPulseCounter = latestRpmPulseCounter;
  previousRpmPulseTime    = latestRpmPulseTime;

  // Do the calcultion and return
  int currentRpm = pulsesPerMinute / rpmPulsesPerRevolution;
  // DEBUG_GENERAL("Current RPM: %d (pulses: %lu, micros: %lu)", currentRpm, latestRpmPulseCounter, latestRpmPulseTime);
  return currentRpm;
}

// Update the RPM scheduler frequency based on the current RPM
void updateRpmSchedulerFrequency(ptScheduler &scheduler, int currentRpm) {
  if (currentRpm >= rpmCalculationFrequencyThreshold && scheduler.sequenceList[0] != PT_TIME_50MS) {
    scheduler.sequenceList[0] = PT_TIME_50MS;
  } else if (currentRpm < rpmCalculationFrequencyThreshold && scheduler.sequenceList[0] != PT_TIME_200MS) {
    scheduler.sequenceList[0] = PT_TIME_200MS;
  }
}
