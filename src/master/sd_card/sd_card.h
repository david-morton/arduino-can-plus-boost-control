#pragma once

#include <SD.h>

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
  const char   *label;
  void         *valuePointer;
  TelemetryType type;
} TelemetryEntry;

/* ======================================================================
   EXTERNALS
   ====================================================================== */

extern TelemetryEntry telemetryMetrics[];
extern const size_t   NUM_TELEMETRY_METRICS;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

bool writeSdTelemetryLogHeader(File *logFileTelemetry);
void writeSdTelemetryLogLine();
