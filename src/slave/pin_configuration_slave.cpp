#include <Arduino.h>

#include "../shared/debug_logging.h"
#include "engine_speed/engine_speed.h"
#include "pin_assignments_slave.h"

/* ======================================================================
   PIN ASSIGNMENTS
   ====================================================================== */

// const byte rpmSignalPin = PIN_TACH_SIGNAL;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Initialise all pins used by the Arduino
void configureAllPins() {
  DEBUG_GENERAL("Configuring all pins for Arduino ...");

  // Set pin modes for MUX control pins
  pinMode(PIN_MUX_S0, OUTPUT);
  pinMode(PIN_MUX_S1, OUTPUT);
  pinMode(PIN_MUX_S2, OUTPUT);
  pinMode(PIN_MUX_S3, OUTPUT);
  pinMode(PIN_MUX_SIGNAL, INPUT);

  // Set pin modes for vehicle sensors and actuators
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_TACH_SIGNAL, INPUT_PULLUP);
  pinMode(PIN_TMAP_PRESSURE_BANK1, INPUT);
  pinMode(PIN_TMAP_PRESSURE_BANK2, INPUT);
  pinMode(PIN_TMAP_PRESSURE_MANIFOLD, INPUT);
  pinMode(PIN_BOOST_SOLENOID_BANK1, OUTPUT);
  pinMode(PIN_BOOST_SOLENOID_BANK2, OUTPUT);

  // Attach the interrupt to the RPM signal pin
  attachInterrupt(digitalPinToInterrupt(PIN_TACH_SIGNAL), updateRpmPulse, RISING);
}
