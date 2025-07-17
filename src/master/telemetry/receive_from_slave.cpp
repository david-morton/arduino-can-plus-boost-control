#include <ptScheduler.h>

#include "../../shared/debug_logging.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "../../shared/telemetry/telemetry_receive_parser.h"

/* ======================================================================
   VARIABLES: From CAN or remote Arduino
   ====================================================================== */

bool currentSwitchStateClutchEngaged;
bool currentSwitchStateInNeutral;
int  currentEngineSpeedRpm      = 0;
int  currentOilPressureGaugeKpa = 0;
int  currentOilTempCelsius      = 0;
int  currentLuxReading          = 0;

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */

float valueFromRemote; // Scratch variable to transport variable values from remote Arduino

/* ======================================================================
   SCHEDULER OBJECTS
   ====================================================================== */

// High frequency tasks (tens of milliseconds)
ptScheduler ptReadCurrentEngineSpeedRpm = ptScheduler(PT_TIME_50MS);

// Medium frequency tasks (hundreds of milliseconds)
ptScheduler ptReadSwitchStateClutch   = ptScheduler(PT_TIME_100MS);
ptScheduler ptReadSwitchStateNeutral  = ptScheduler(PT_TIME_100MS);

// Low frequency tasks (seconds)
ptScheduler ptGetCurrentLuxReading      = ptScheduler(PT_TIME_2S);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Handle telemetry data received from remote Arduino
void handleTelemetryReceivedFromSlave() {
    // Update the current lux reading from the remote Arduino
    if (ptGetCurrentLuxReading.call() && handleTelemetryFloat(SENSOR_LUX, &valueFromRemote)) {
    currentLuxReading = valueFromRemote;
    }

    // Update the current clutch and neutral statuses from the remote Arduino
    if (ptReadSwitchStateClutch.call() && handleTelemetryFloat(SENSOR_CLUTCH, &valueFromRemote)) {
    currentSwitchStateClutchEngaged = valueFromRemote;
    }

    if (ptReadSwitchStateNeutral.call() && handleTelemetryFloat(SENSOR_NEUTRAL, &valueFromRemote)) {
    currentSwitchStateInNeutral = valueFromRemote;
    }

    // Update the current RPM reading from the remote Arduino
    // If the remote Arduino is not sending new RPM data, this will not update
    if (ptReadCurrentEngineSpeedRpm.call() && handleTelemetryFloat(SENSOR_RPM, &valueFromRemote)) {
    currentEngineSpeedRpm = valueFromRemote;
    }
}
