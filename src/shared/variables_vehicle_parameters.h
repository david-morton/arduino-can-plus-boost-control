#include "Arduino.h"

/* ======================================================================
   VARIABLES: Physical sensor inputs read from mux on slave Arduino
   ====================================================================== */

extern bool currentSwitchStateClutch;      // Mux channel digital
extern bool currentSwitchStateNeutral;     // Mux channel digital
extern int  currentBrakePressureFrontKpa;  // Mux channel analogue
extern int  currentBrakePressureRearKpa;   // Mux channel analogue
extern int  currentCoolantTempCelsius;     // Mux channel voltage divided 1.5k ?
extern int  currentCrankCaseVacuumKpa;     // Mux channel analogue
extern int  currentFuelPressureKpa;        // Mux channel analogue
extern int  currentIntakeTempBank1Celsius; // Mux channel voltage divided 1.2k
extern int  currentIntakeTempBank2Celsius; // Mux channel voltage divided 1.2k
extern int  currentIntakeTempManifoldCels; // Mux channel voltage divided 1.2k
extern int  currentOilPressureKpa;         // Mux channel analogue
extern int  currentOilTempCelsius;         // Mux channel voltage divided 1.5k ?

/* ======================================================================
   VARIABLES: Physical sensor inputs read directly on slave Arduino
   ====================================================================== */

extern int currentAmbientLux; // I2C Light sensor

extern int currentIntakePressureBank1Kpa;    // Analogue Bosch 3 bar TMAP
extern int currentIntakePressureBank2Kpa;    // Analogue Bosch 3 bar TMAP
extern int currentIntakePressureManifoldKpa; // Analogue Bosch 3 bar TMAP
extern int currentEngineSpeedRpm;            // Interrupt measured square wave from Nissan ECU

/* ======================================================================
   VARIABLES: Values populated on master Arduino
   ====================================================================== */

extern float currentVehicleSpeedFrontKph;
extern float currentVehicleSpeedRearKph;
extern int   currentElectronicsTempCelcius;
extern int   currentEngineTempCelcius;

/* ======================================================================
   VARIABLES: Derived or calculated values on master Arduino
   ====================================================================== */

extern int currentBoostTargetKpa;
extern int currentGear;
extern int currentRadiatorFanDutyPercentage;
