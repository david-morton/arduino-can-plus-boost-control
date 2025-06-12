#include <Arduino.h>

#include "../shared/debug_logging.h"
#include "engine_speed/engine_speed.h"
#include "pin_assignments_slave.h"

/* ======================================================================
   PIN ASSIGNMENTS
   ====================================================================== */

// const byte rpmSignalPin = ARDUINO_PIN_TACH_SIGNAL;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Initialise all pins used by the Arduino
void configureAllPins() {
  DEBUG_GENERAL("Configuring all pins for Arduino ...");

  // Set pin modes for MUX control pins
  pinMode(ARDUINO_MUX_S0, OUTPUT);
  pinMode(ARDUINO_MUX_S1, OUTPUT);
  pinMode(ARDUINO_MUX_S2, OUTPUT);
  pinMode(ARDUINO_MUX_S3, OUTPUT);
  pinMode(ARDUINO_PIN_MUX_SIGNAL, INPUT);

  // Set pin modes for vehicle sensors and actuators
  pinMode(ARDUINO_PIN_BUZZER, OUTPUT);
  pinMode(ARDUINO_PIN_TACH_SIGNAL, INPUT_PULLUP);
  pinMode(ARDUINO_BOOST_SOLENOID_BANK1, OUTPUT);
  pinMode(ARDUINO_BOOST_SOLENOID_BANK2, OUTPUT);

  // Attach the interrupt to the RPM signal pin
  attachInterrupt(digitalPinToInterrupt(ARDUINO_PIN_TACH_SIGNAL), updateRpmPulse, RISING);
}
