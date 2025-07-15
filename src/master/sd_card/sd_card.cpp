#include <SD.h>

#include "../../shared/debug_logging.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "sd_card.h"
#include "sd_helpers.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

TelemetryEntry telemetryMetrics[] = {
    {"RPM", &currentEngineSpeedRpm, TYPE_INT},
    {"Front Speed", &currentVehicleSpeedFrontKph, TYPE_FLOAT},
    {"Rear Speed", &currentVehicleSpeedRearKph, TYPE_FLOAT},
    {"Engine Temp", &currentEngineTempCelcius, TYPE_INT},
    {"Clutch Engaged", &currentSwitchStateClutch, TYPE_BOOL},
};

const size_t NUM_TELEMETRY_METRICS = sizeof(telemetryMetrics) / sizeof(telemetryMetrics[0]);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Write header row for telemetry log file
bool writeSdTelemetryLogHeader(File *file) {
  if (!file)
    return false;

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
