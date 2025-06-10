#pragma once

#include "Arduino.h"

/* ======================================================================
   PIN CONSTANTS - MUX CHANNELS
   ====================================================================== */

const byte MUX_CHANNEL_CLUTCH_SWITCH  = 5;
const byte MUX_CHANNEL_NEUTRAL_SWITCH = 6;

/* ======================================================================
   PIN CONSTANTS - ARDUINO DIGITAL
   ====================================================================== */

const byte ARDUINO_PIN_TACH_SIGNAL      = 2;
const byte ARDUINO_PIN_BUZZER           = 3;
const byte ARDUINO_MUX_S0               = 4;
const byte ARDUINO_BOOST_SOLENOID_BANK1 = 5;
const byte ARDUINO_BOOST_SOLENOID_BANK2 = 6;
const byte ARDUINO_MUX_S1               = 7;
const byte ARDUINO_MUX_S2               = 8;
const byte ARDUINO_MUX_S3               = 9;

/* ======================================================================
   PIN CONSTANTS - ARDUINO ANALOGUE
   ====================================================================== */

const byte ARDUINO_PIN_MUX_SIGNAL = A3;
