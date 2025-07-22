#pragma once

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern bool currentSwitchStateClutchEngagedFromSlave;
extern bool currentSwitchStateInNeutralFromSlave;

extern float currentIntakePressureBank1GaugeKpaFromSlave;
extern float currentIntakePressureBank2GaugeKpaFromSlave;
extern float currentIntakePressureManifoldGaugeKpaFromSlave;

extern int currentEngineSpeedRpmFromSlave;
extern int currentLuxReadingFromSlave;

/* ======================================================================
FUNCTION PROTOTYPES
====================================================================== */

void handleTelemetryReceivedFromSlave();
