#pragma once

/* ======================================================================
   CONSTANTS
   ====================================================================== */

constexpr uint8_t PIN_TMAP_PRESSURE_BANK1    = A0; // Pin for Bank 1 TMAP sensor
constexpr uint8_t PIN_TMAP_PRESSURE_BANK2    = A1; // Pin for Bank 2 TMAP sensor
constexpr uint8_t PIN_TMAP_PRESSURE_MANIFOLD = A2; // Pin for Manifold TMAP sensor

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void handleBoosControlTasks();
