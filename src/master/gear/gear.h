#pragma once

/* ======================================================================
   CONSTANTS AND DEFINITIONS
   ====================================================================== */

// Tyre details
constexpr float TYRE_WIDTH_MM     = 255.0;
constexpr float TYRE_ASPECT_RATIO = 0.35;
constexpr float WHEEL_DIAMETER_IN = 18.0;

// Final drive ratio
constexpr float FINAL_DRIVE_RATIO = 3.38;

// Transmission gear ratios
constexpr int   NUM_GEARS              = 6;
constexpr float GEAR_RATIOS[NUM_GEARS] = {
    3.79, 2.32, 1.62, 1.27, 1.00, 0.79};

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern int currentGear;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void  updateCurrentGear();
float calculateTyreCircumference();
void  reportTyreCircumference();
