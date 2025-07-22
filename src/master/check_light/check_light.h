#pragma once

/* ======================================================================
   ENUM DEFINITIONS
   ====================================================================== */

// Check light state enumeration
typedef enum {
  CHECK_LIGHT_OFF  = 0,
  CHECK_LIGHT_ON   = 2,
  CHECK_LIGHT_EML  = 16,
  CHECK_LIGHT_BOTH = 18
} CheckLightState;

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern CheckLightState currentCheckEngineLightState;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void handleCheckLightState();
void updateCheckLightStatus();
