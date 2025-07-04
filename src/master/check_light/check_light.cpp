#include <Arduino.h>

#include "check_light.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

// Define enumeration for check light states, default to on for startup
CheckLightState currentCheckEngineLightState = CHECK_LIGHT_ON;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Update the status of the check light based on the current engine speed and temperature
void updateCheckLightStatus() {

  if (millis() < 8000) {
    currentCheckEngineLightState = CHECK_LIGHT_ON; // Keep the check light on for the first 8 seconds after startup
  } else {
    handleCheckLightState();
  }
}

// Update the check light state based on qualifying conditions
void handleCheckLightState() {
  currentCheckEngineLightState = CHECK_LIGHT_OFF; // Turn off the check light after 8 seconds
                                                  // TODO: Implement proper check light logic here later
}
