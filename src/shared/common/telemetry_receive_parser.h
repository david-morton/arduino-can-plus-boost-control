#pragma once

#include "telemetry_send_staging.h"

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void parseTelemetryPayload(const char *payload);
bool consumeTelemetryFloat(TelemetryField field, float *outValue);
