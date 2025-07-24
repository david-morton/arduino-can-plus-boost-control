#pragma once

#include <Arduino.h>

/* ======================================================================
   PIN CONSTANTS - MUX CHANNELS
   ====================================================================== */

constexpr uint8_t MUX_CHANNEL_CLUTCH_SWITCH  = 5;
constexpr uint8_t MUX_CHANNEL_NEUTRAL_SWITCH = 6;

/* ======================================================================
   PIN CONSTANTS - ARDUINO DIGITAL
   ====================================================================== */

constexpr uint8_t PIN_TACH_SIGNAL          = 2;
constexpr uint8_t PIN_BUZZER               = 3;
constexpr uint8_t PIN_ETHERNET_CS          = 4;
constexpr uint8_t PIN_MUX_S0               = 5;
constexpr uint8_t PIN_MUX_S1               = 6;
constexpr uint8_t PIN_MUX_S2               = 7;
constexpr uint8_t PIN_MUX_S3               = 8;
constexpr uint8_t PIN_BOOST_SOLENOID_BANK1 = 9;
constexpr uint8_t PIN_BOOST_SOLENOID_BANK2 = 10;

/* ======================================================================
   PIN CONSTANTS - ARDUINO ANALOGUE
   ====================================================================== */

constexpr uint8_t PIN_TMAP_PRESSURE_BANK1    = A0;
constexpr uint8_t PIN_TMAP_PRESSURE_BANK2    = A1;
constexpr uint8_t PIN_TMAP_PRESSURE_MANIFOLD = A2;
constexpr uint8_t PIN_MUX_SIGNAL             = A3;
