#include "Arduino.h"
#include "RTClib.h"

#include "../../shared/debug_logging.h"
#include "rtc_helpers.h"


RTC_DS3231 rtc;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Initialise the RTC module
void initialiseRtc() {
  DEBUG_GENERAL("INFO - Initialising RTC ...");

  if (!rtc.begin()) {
    DEBUG_ERROR("RTC not found. Please check the wiring.");
  } else {
    DEBUG_GENERAL("\tOK - RTC initialised successfully");
  }
}

// Get the current date and time from the RTC and return it as a DateTime object
DateTime getRtcCurrentDateTime() {
  DateTime now = rtc.now();
  if (!now.isValid()) {
    DEBUG_ERROR("RTC returned an invalid date/time.");
    return DateTime(); // Return an invalid DateTime object
  }
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

  return tempC; // Return temperature in degrees Celsius
}
