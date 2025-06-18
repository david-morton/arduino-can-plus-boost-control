#include <Arduino.h>

#include "../../shared/debug_logging.h"
#include "../pin_assignments_slave.h"
#include "alarm_helpers.h"

/* ======================================================================
   PIN DEFINITIONS
   ====================================================================== */

const byte alarmBuzzerPin = ARDUINO_PIN_BUZZER;

/* ======================================================================
   VARIABLES
   ====================================================================== */

static unsigned long inAlarmDuration    = 0;
static unsigned long firstAlarmCallTime = 0;
static bool          alarmIsSounding    = false;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Perform short alarm buzzer beep to ensure its working
void performAlarmBuzzerStartupBeep() {
  DEBUG_GENERAL("Performing alarm buzzer startup beep ...");
  tone(alarmBuzzerPin, 4000, 1500);
}

// Disable the alarm
void alarmDisable() {
  // Return quickly if alarm is not sounding
  if (!alarmIsSounding) {
    return;
  }
  noTone(alarmBuzzerPin);
  firstAlarmCallTime = 0;
  alarmIsSounding    = false;
  DEBUG_ERROR("Alarm buzzer disabled, in alarm duration: %lu ms", inAlarmDuration);
}

// Sound the alarm if the engine is running, and after a delay
bool alarmEnable(int engineRpm) {
  if (firstAlarmCallTime == 0) {
    firstAlarmCallTime = millis();
  }

  inAlarmDuration = millis() - firstAlarmCallTime;

  if (engineRpm > 500 && inAlarmDuration > 1000 && !alarmIsSounding) {
    tone(alarmBuzzerPin, 4000);
    alarmIsSounding = true;
    DEBUG_ERROR("Alarm buzzer enabled, engine was RPM: %d, in alarm duration: %lu ms", engineRpm, inAlarmDuration);
  } else if (engineRpm == 0) {
    alarmDisable();
  }
}
