#pragma once

#include <Arduino.h>

/* ======================================================================
   CONSTANTS
   ====================================================================== */

// Define the moving window array size for light readings
constexpr int LIGHT_SENSOR_READINGS_SIZE = 5;

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern int currentAmbientLux;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void initialiseAmbientLightSensor();
void readAmbientLightSensor();
int  calculateAverageLux();
