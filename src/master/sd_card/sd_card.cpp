#include <SD.h>

#include "../../shared/debug_logging.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "sd_card.h"
#include "sd_helpers.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

// Telemetry metrics to log on the SD card, including a friendly name
TelemetryEntry telemetryMetrics[] = {
    {"RPM", &currentEngineSpeedRpm, TYPE_INT},
    {"Front Speed", &currentVehicleSpeedFrontKph, TYPE_FLOAT},
    {"Rear Speed", &currentVehicleSpeedRearKph, TYPE_FLOAT},
    {"Engine Temp", &currentEngineTempCelcius, TYPE_INT},
    {"Clutch Engaged", &currentSwitchStateClutchEngaged, TYPE_BOOL},
    {"In Neutral", &currentSwitchStateInNeutral, TYPE_BOOL},
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

  file->print("Timestamp,");
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
  getIso8601Timestamp(timestampBuffer, sizeof(timestampBuffer));
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
