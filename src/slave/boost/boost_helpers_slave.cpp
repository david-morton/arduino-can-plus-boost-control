#include <Arduino.h>

#include "../../shared/debug_logging.h"
#include "../../shared/variables_vehicle_parameters.h"

/* ======================================================================
   CONSTANTS
   ====================================================================== */

// Constants for the Bosch 3 Bar TMAP pressure sensor calculations
// as per https://www.bosch-motorsport-shop.com.au/t-map-sensor-3-bar-130-deg-c
constexpr float c0 = 5.4f / 280.0f;
constexpr float c1 = 0.85f / 280.0f;

// Supply voltage for the sensor, typically 5V for automotive sensors
constexpr float sensorSupplyVoltage = 5.0f;

// ADC resolution for Arduino (10-bit ADC yields values from 0–1023)
constexpr float adcResolution = 1023.0f;

/* ======================================================================
   VARIABLES
   ====================================================================== */

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Read the Bosch 3 Bar TMAP pressure sensor value from the specified pin and return the gauge pressure in kPa
int readBosch3BarTmapPressure(uint8_t pin) {
  float pinVoltage       = analogRead(pin) * (sensorSupplyVoltage / adcResolution); // Convert ADC reading to voltage
  pinVoltage             = constrain(pinVoltage, 0.0f, sensorSupplyVoltage);        // Clamp to expected range
  float pressureAbsolute = ((pinVoltage / sensorSupplyVoltage) - c0) / c1;
  float pressureGaugeKpa = pressureAbsolute - 100.0f; // Convert to gauge pressure
  return static_cast<int>(pressureGaugeKpa);
}
