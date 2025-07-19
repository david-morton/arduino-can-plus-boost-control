#pragma once

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern double boostTargetGaugeKpa;
extern double currentIntakePressureBank1GaugeKpa;    // Analogue Bosch 3 bar TMAP
extern double currentIntakePressureBank2GaugeKpa;    // Analogue Bosch 3 bar TMAP
extern double currentIntakePressureManifoldGaugeKpa; // Analogue Bosch 3 bar TMAP

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void handleBoostControlTasks();
