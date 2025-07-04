#include <Arduino.h>
#include <hp_BH1750.h> // Adafruit library for BH1750

#include "../../shared/debug_logging.h"
#include "../../shared/variables_programmatic.h"
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
  DEBUG_GENERAL("Initialising BH1750 light sensor ...");

  bool avail = BH1750.begin(BH1750_TO_GROUND); // Sensor address pin tied to GND = 0x23

  if (avail) {
    sensorAvailable = true;
    BH1750.start(); // Start the first measurement in setup since we will be using continuous mode
    DEBUG_GENERAL("\t\tBH1750 light sensor initialized successfully.");
  } else {
    sensorAvailable = false;
    DEBUG_ERROR("\t\tFailed to initialize BH1750 light sensor! Sensor unavailable.");
  }
}

// Store a lux reading in the circular buffer
void handleLuxReading(int lux) {
  lightSensorReadings[lightSensorReadingIndex] = lux;
  lightSensorReadingIndex                      = (lightSensorReadingIndex + 1) % LIGHT_SENSOR_READINGS_SIZE;
}

// Read the ambient light sensor value and store it in circular buffer
void readAmbientLightSensor() {
  if (!sensorAvailable) {
    handleLuxReading(-1); // Store 0 if sensor is not available
    DEBUG_ERROR("Cannot read ambient light sensor, sensor not available.");
    return;
  }

  if (BH1750.hasValue()) {
    int lux = BH1750.getLux();
    BH1750.start(); // Trigger next measurement
    handleLuxReading(lux);
    DEBUG_SENSOR_READINGS("Ambient light sensor reading: %d lux (stored at index %d)", lux, lightSensorReadingIndex);
  } else {
    handleLuxReading(-1); // Store 0 if no valid reading
    DEBUG_ERROR("No valid reading from ambient light sensor.");
  }
}

// Get the average lux value from the circular buffer
int calculateAverageLux() {
  readAmbientLightSensor(); // Ensure we have the latest reading before calculating average

  int sum = 0;
  for (int i = 0; i < LIGHT_SENSOR_READINGS_SIZE; i++) {
    sum += lightSensorReadings[i];
  }
  int average = sum / LIGHT_SENSOR_READINGS_SIZE;

  // Return a default value if average is negative else return the calculated average
  if (average < 0) {
    DEBUG_ERROR("Average lux value is negative, defaulting to %d.", defaultLuxValue);
    return defaultLuxValue;
  } else {
    DEBUG_SENSOR_READINGS("Average lux value calculated from %d readings: %d", LIGHT_SENSOR_READINGS_SIZE, average);
    return average;
  }
}
