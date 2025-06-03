#pragma once

#include "telemetry_staging.h"

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void parseTelemetryPayload(const char *payload);
bool consumeTelemetryFloat(TelemetryField field, float* outValue);
