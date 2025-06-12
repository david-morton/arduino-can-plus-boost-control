#pragma once

#include <Arduino.h>

// Define the moving window array size for light readings
#define LIGHT_SENSOR_READINGS_SIZE 5

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void initialiseAmbientLightSensor();
void readAmbientLightSensor();
int  calculateAverageLux();
