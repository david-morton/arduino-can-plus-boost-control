#include <Arduino.h>

/* ======================================================================
   VARIABLES: Physical sensor inputs read from mux on slave Arduino
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

/* ======================================================================
   VARIABLES: Physical sensor inputs read directly on master Arduino
   ====================================================================== */

extern int currentElectronicsRtcTemp; // Read from RTC sensor on master Arduino

/* ======================================================================
   VARIABLES: Physical sensor inputs read directly on slave Arduino
   ====================================================================== */

extern int currentAmbientLux;     // I2C Light sensor
extern int currentEngineSpeedRpm; // Interrupt measured square wave from Nissan ECU

/* ======================================================================
   VARIABLES: Values populated on master Arduino
   ====================================================================== */

extern float currentVehicleSpeedFrontKph;
extern float currentVehicleSpeedRearKph;
extern int   currentElectronicsRtcTempCelcius;
extern int   currentEngineTempCelcius;

/* ======================================================================
   VARIABLES: Derived or calculated values on master Arduino
   ====================================================================== */

extern int currentCheckEngineLightState;
extern int currentRadiatorFanDutyPercentage;