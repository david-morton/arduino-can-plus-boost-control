#pragma once

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern float currentIntakePressureBank1GaugeKpa;
extern float currentIntakePressureBank2GaugeKpa;
extern float currentIntakePressureManifoldGaugeKpa;

/* ======================================================================
FUNCTION PROTOTYPES
====================================================================== */

void handleTelemetryReceivedFromSlave();
