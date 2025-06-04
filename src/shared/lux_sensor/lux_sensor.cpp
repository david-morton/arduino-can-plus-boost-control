#include <Arduino.h>
#include <hp_BH1750.h> // Adafruit library for BH1750

#include "../common/debug_logging.h"
#include "../common/variables.h"
#include "lux_sensor.h"

/* ======================================================================
   CONFIGURE CLIENT
   ====================================================================== */

hp_BH1750 BH1750; //  create the sensor object

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */

bool sensorAvailable                                 = false; // Flag to indicate if the sensor is available
int  luxValue                                        = 0;     // Variable to store the lux value read from the sensor
int  lightSensorReadings[LIGHT_SENSOR_READINGS_SIZE] = {0};   // Array to store multiple readings
int  lightSensorReadingIndex                         = 0;     // Current index in circular buffer
int  defaultLuxValue                                 = 200;   // Default lux value to return if sensor reading is invalid

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Initialise the BH1750 ambient light sensor
void initialiseAmbientLightSensor() {
  DEBUG_GENERAL("INFO: Initialising BH1750 light sensor ...");

  bool avail = BH1750.begin(BH1750_TO_GROUND); // Sensor address pin tied to GND = 0x23

  if (avail) {
    sensorAvailable = true;
    BH1750.start(); // Start the first measurement in setup since we will be using continuous mode
    DEBUG_GENERAL("INFO: BH1750 light sensor initialized successfully.");
  } else {
    sensorAvailable = false;
    DEBUG_ERROR("ERROR: Failed to initialize BH1750 light sensor! Sensor unavailable.");
  }
}

// Store a lux reading in the circular buffer
void storeLuxReading(int lux) {
  lightSensorReadings[lightSensorReadingIndex] = lux;
  lightSensorReadingIndex                      = (lightSensorReadingIndex + 1) % LIGHT_SENSOR_READINGS_SIZE;
}

// Read the ambient light sensor value and store it in circular buffer
void readAmbientLightSensor() {
  if (!sensorAvailable) {
    storeLuxReading(-1); // Store 0 if sensor is not available
    DEBUG_ERROR("ERROR: Cannot read ambient light sensor, sensor not available.");
    return;
  }

  if (BH1750.hasValue()) {
    int lux = BH1750.getLux();
    BH1750.start(); // Trigger next measurement
    storeLuxReading(lux);
    DEBUG_SENSOR_READINGS("INFO: Ambient light sensor reading: %d lux (stored at index %d)", lux, lightSensorReadingIndex);
  } else {
    storeLuxReading(-1); // Store 0 if no valid reading
    DEBUG_ERROR("ERROR: No valid reading from ambient light sensor.");
  }
}

// Get the average lux value from the circular buffer
int getAverageLux() {
  readAmbientLightSensor(); // Ensure we have the latest reading before calculating average

  int sum = 0;
  for (int i = 0; i < LIGHT_SENSOR_READINGS_SIZE; i++) {
    sum += lightSensorReadings[i];
  }
  int average = sum / LIGHT_SENSOR_READINGS_SIZE;

  // Return a default value if average is negative else return the calculated average
  if (average < 0) {
    DEBUG_ERROR("ERROR: Average lux value is negative, defaulting to %d.", defaultLuxValue);
    return defaultLuxValue;
  } else {
    DEBUG_SENSOR_READINGS("INFO: Average lux value calculated from %d readings: %d", LIGHT_SENSOR_READINGS_SIZE, average);
    return average;
  }
}
