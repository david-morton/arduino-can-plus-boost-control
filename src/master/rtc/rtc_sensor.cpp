#include <Arduino.h>
#include <RTClib.h>

#include "../../shared/debug_logging.h"
#include "rtc_sensor.h"

RTC_DS3231 rtc;

/* ======================================================================
   VARIABLES
   ====================================================================== */

int currentElectronicsRtcTemp = 0; // Variable to store the current electronics temperature reading from RTC

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Initialise the RTC module
void initialiseRtc() {
  DEBUG_GENERAL("Initialising RTC ...");

  if (!rtc.begin()) {
    DEBUG_ERROR("\tRTC not found. Please check the wiring.");
  } else {
    DateTime now = getRtcCurrentDateTime();
    DEBUG_GENERAL("\t\tRTC initialised successfully. Current RTC time: %04d-%02d-%02d %02d:%02d:%02d",
                  now.year(), now.month(), now.day(),
                  now.hour(), now.minute(), now.second());
  }
}

// Get the current date and time from the RTC and return it as a DateTime object
DateTime getRtcCurrentDateTime() {
  DateTime now = rtc.now();
  if (!now.isValid()) {
    DEBUG_ERROR("RTC returned an invalid date/time.");
    return DateTime(); // Return an invalid DateTime object
  }
  // Format: YYYY-MM-DD HH:MM:SS
  DEBUG_SENSOR_READINGS("RTC Date/Time: %04d-%02d-%02d %02d:%02d:%02d",
                        now.year(), now.month(), now.day(),
                        now.hour(), now.minute(), now.second());
  return now;
}

// Get the current temperature from the RTC in celcius
float getRtcCurrentTemperature() {
  if (!rtc.isEnabled32K()) {
    DEBUG_ERROR("RTC temperature sensor is not enabled.");
    return NAN; // Return an error value
  }

  float tempC = rtc.getTemperature();

  if (tempC == NAN) {
    DEBUG_ERROR("Failed to read temperature from RTC.");
    return -99; // Return an error value
  }

  DEBUG_SENSOR_READINGS("RTC Temperature %.1fC", tempC);
  return tempC; // Return temperature in degrees Celsius
}
