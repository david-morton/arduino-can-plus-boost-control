#pragma once

#include <SD.h>
#include "../../shared/variables_vehicle_parameters.h"

/* ======================================================================
   ENUMS
   ====================================================================== */

enum TelemetryType {
  TYPE_FLOAT,
  TYPE_INT,
  TYPE_BOOL
};

/* ======================================================================
   STRUCTS
   ====================================================================== */

typedef struct {
  const char *label;
  void *valuePointer;
  TelemetryType type;
} TelemetryEntry;

/* ======================================================================
   EXTERNALS
   ====================================================================== */

extern TelemetryEntry telemetryMetrics[];
extern const size_t NUM_TELEMETRY_METRICS;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void writeSdTelemetryLogLine();
bool writeSdTelemetryLogHeader(File *logFileTelemetry);
