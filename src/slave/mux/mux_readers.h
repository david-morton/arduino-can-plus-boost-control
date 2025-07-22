#pragma once

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern bool currentSwitchStateClutchEngaged;   // Mux channel digital
extern bool currentSwitchStateInNeutral;       // Mux channel digital
extern int  currentBrakePressureFrontGaugeKpa; // Mux channel analogue
extern int  currentBrakePressureRearGaugeKpa;  // Mux channel analogue
extern int  currentCoolantTempCelsius;         // Mux channel voltage divided 1.5k ?
extern int  currentCrankCaseVacuumGaugeKpa;    // Mux channel analogue
extern int  currentFuelPressureGaugeKpa;       // Mux channel analogue
extern int  currentIntakeTempBank1Celsius;     // Mux channel voltage divided 1.2k
extern int  currentIntakeTempBank2Celsius;     // Mux channel voltage divided 1.2k
extern int  currentIntakeTempManifoldCels;     // Mux channel voltage divided 1.2k
extern int  currentOilPressureGaugeKpa;        // Mux channel analogue
extern int  currentOilTempCelsius;             // Mux channel voltage divided 1.5k ?
