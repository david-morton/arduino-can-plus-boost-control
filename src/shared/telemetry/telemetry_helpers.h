#pragma once

#include "telemetry_send_staging.h"

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

TelemetryField getTelemetryFieldForKey(const char *key);
const char    *getTelemetryKeyForField(TelemetryField field);
