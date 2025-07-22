#include <ptScheduler.h>

#include "../../shared/debug_logging.h"
#include "../../shared/telemetry/telemetry_receive_parser.h"

/* ======================================================================
   VARIABLES: From CAN or remote Arduino
   ====================================================================== */

bool currentSwitchStateClutchEngagedFromSlave;
bool currentSwitchStateInNeutralFromSlave;
int  currentEngineSpeedRpmFromSlave = 0;
int  currentLuxReadingFromSlave     = 0;

float currentIntakePressureBank1GaugeKpaFromSlave    = 0;
float currentIntakePressureBank2GaugeKpaFromSlave    = 0;
float currentIntakePressureManifoldGaugeKpaFromSlave = 0;

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */

float valueFromRemote; // Scratch variable to transport variable values from remote Arduino

/* ======================================================================
   SCHEDULER OBJECTS
   ====================================================================== */

// High frequency tasks (tens of milliseconds)
ptScheduler ptGetCurrentEngineSpeedRpm = ptScheduler(PT_TIME_50MS);

// Medium frequency tasks (hundreds of milliseconds)
ptScheduler ptGetSwitchStateClutch        = ptScheduler(PT_TIME_100MS);
ptScheduler ptGetSwitchStateNeutral       = ptScheduler(PT_TIME_100MS);
ptScheduler ptGetCurrentBoostMeasurements = ptScheduler(PT_TIME_100MS);

// Low frequency tasks (seconds)
ptScheduler ptGetCurrentLuxReading = ptScheduler(PT_TIME_2S);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Handle staged telemetry data received from remote Arduino (slave)
void handleTelemetryReceivedFromSlave() {
  // Update the current lux reading from the remote Arduino
  if (ptGetCurrentLuxReading.call() && handleTelemetryFloat(TM_LUX, &valueFromRemote)) {
    currentLuxReadingFromSlave = valueFromRemote;
  }

  // Update the current clutch and neutral statuses from the remote Arduino
  if (ptGetSwitchStateClutch.call() && handleTelemetryFloat(TM_CLUTCH_IN, &valueFromRemote)) {
    currentSwitchStateClutchEngagedFromSlave = valueFromRemote;
  }

  if (ptGetSwitchStateNeutral.call() && handleTelemetryFloat(TM_IN_NEUTRAL, &valueFromRemote)) {
    currentSwitchStateInNeutralFromSlave = valueFromRemote;
  }

  // Update the current boost measurements from the remote Arduino
  if (ptGetCurrentBoostMeasurements.call()) {
    if (handleTelemetryFloat(TM_BOOST_BANK1, &valueFromRemote)) {
      currentIntakePressureBank1GaugeKpaFromSlave = valueFromRemote;
    }
    if (handleTelemetryFloat(TM_BOOST_BANK2, &valueFromRemote)) {
      currentIntakePressureBank2GaugeKpaFromSlave = valueFromRemote;
    }
    if (handleTelemetryFloat(TM_BOOST_MANIFOLD, &valueFromRemote)) {
      currentIntakePressureManifoldGaugeKpaFromSlave = valueFromRemote;
    }
  }

  // Update the current RPM reading from the remote Arduino
  // If the remote Arduino is not sending new RPM data, this will not update
  if (ptGetCurrentEngineSpeedRpm.call() && handleTelemetryFloat(TM_RPM, &valueFromRemote)) {
    currentEngineSpeedRpmFromSlave = valueFromRemote;
  }
}
