#include <SD.h>

#include "../../shared/debug_logging.h"
#include "../boost/boost_helpers_master.h"
#include "../can/can_receive.h"
#include "../gear/gear.h"
#include "../telemetry/receive_from_slave.h"
#include "sd_card.h"
#include "sd_helpers.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

// Telemetry metrics to log on the SD card, including a friendly name
TelemetryEntry telemetryMetrics[] = {
    {"RPM", &currentEngineSpeedRpmFromSlave, TYPE_INT},
    {"Front Speed", &currentVehicleSpeedFrontKph, TYPE_FLOAT},
    {"Rear Speed", &currentVehicleSpeedRearKph, TYPE_FLOAT},
    {"Engine Temp", &currentEngineTempCelcius, TYPE_INT},
    {"Boost Recommended", &recommendedBoostTargetGaugeKpa, TYPE_INT},
    {"Gear", &currentGear, TYPE_INT},
    {"Boost Bank 1", &currentIntakePressureBank1GaugeKpaFromSlave, TYPE_FLOAT},
    {"Boost Bank 2", &currentIntakePressureBank2GaugeKpaFromSlave, TYPE_FLOAT},
    {"Boost Manifold", &currentIntakePressureManifoldGaugeKpaFromSlave, TYPE_FLOAT},
    {"Clutch Engaged", &currentSwitchStateClutchEngagedFromSlave, TYPE_BOOL},
    {"In Neutral", &currentSwitchStateInNeutralFromSlave, TYPE_BOOL},
};

// Number of telemetry metrics defined above
const size_t NUM_TELEMETRY_METRICS = sizeof(telemetryMetrics) / sizeof(telemetryMetrics[0]);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Write header row for telemetry log file
bool writeSdTelemetryLogHeader(File *file) {
  if (!file)
    return false;

  file->print("Time,");
  for (size_t i = 0; i < NUM_TELEMETRY_METRICS; ++i) {
    file->print(telemetryMetrics[i].label);
    if (i < NUM_TELEMETRY_METRICS - 1)
      file->print(",");
  }
  file->println();
  return true;
}

// Write latest data to the telemetry log file.
void writeSdTelemetryLogLine() {
  if (!logFileTelemetry)
    return;

  char timestampBuffer[32];
  generateLogTimestamp(timestampBuffer, sizeof(timestampBuffer));
  logFileTelemetry.print(timestampBuffer);
  logFileTelemetry.print(",");

  for (size_t i = 0; i < NUM_TELEMETRY_METRICS; ++i) {
    switch (telemetryMetrics[i].type) {
      case TYPE_FLOAT:
        logFileTelemetry.print(*(float *)(telemetryMetrics[i].valuePointer), 2);
        break;
      case TYPE_INT:
        logFileTelemetry.print(*(int *)(telemetryMetrics[i].valuePointer));
        break;
      case TYPE_BOOL:
        logFileTelemetry.print(*(bool *)(telemetryMetrics[i].valuePointer) ? "1" : "0");
        break;
    }

    if (i < NUM_TELEMETRY_METRICS - 1)
      logFileTelemetry.print(",");
  }

  logFileTelemetry.println();
}
