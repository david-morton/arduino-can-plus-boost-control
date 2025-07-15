#include <SD.h>

#include "sd_helpers.h"
#include "sd_card.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "../../shared/debug_logging.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

File logFileTelemetry;
unsigned long lastFlushMillis = 0;
const unsigned long FLUSH_INTERVAL_MS = 5000; // flush every 5 seconds

TelemetryEntry telemetryMetrics[] = {
  {"Front Speed (kph)",     &currentVehicleSpeedFrontKph,    TYPE_FLOAT},
  {"Rear Speed (kph)",      &currentVehicleSpeedRearKph,     TYPE_FLOAT},
  {"Engine Temp (°C)",      &currentEngineTempCelcius,       TYPE_INT},
  {"Clutch Engaged",        &currentSwitchStateClutch,       TYPE_BOOL},
};

const size_t NUM_TELEMETRY_METRICS = sizeof(telemetryMetrics) / sizeof(telemetryMetrics[0]);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Write header row for telemetry log file
bool writeSdTelemetryLogHeader(File *file) {
  if (!file) return false;

  for (size_t i = 0; i < NUM_TELEMETRY_METRICS; ++i) {
    file->print(telemetryMetrics[i].label);
    if (i < NUM_TELEMETRY_METRICS - 1) file->print(",");
  }
  file->println();
  return true;
}


// Write latest data to the telemetry log file.
void writeSdTelemetryLogLine() {
  if (!logFileTelemetry) {
    logFileTelemetry = SD.open(telemetryLogFilename, FILE_WRITE);
    if (!logFileTelemetry) {
      DEBUG_ERROR("Failed to reopen telemetry log file.");
      sdReadyToLogTelemetry = false;
      return;
    }
  }

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

    if (i < NUM_TELEMETRY_METRICS - 1) logFileTelemetry.print(",");
  }

  logFileTelemetry.println();

  // Periodically flush to disk
  unsigned long now = millis();
  if (now - lastFlushMillis >= FLUSH_INTERVAL_MS) {
    logFileTelemetry.flush();
    lastFlushMillis = now;
  }
}
