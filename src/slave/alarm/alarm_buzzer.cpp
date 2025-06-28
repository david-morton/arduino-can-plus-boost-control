#include <Arduino.h>

#include "../../shared/debug_logging.h"
#include "../pin_assignments_slave.h"
#include "alarm_buzzer.h"

/* ======================================================================
   PIN DEFINITIONS
   ====================================================================== */

const byte alarmBuzzerPin = ARDUINO_PIN_BUZZER;

/* ======================================================================
   VARIABLES
   ====================================================================== */

bool                 alarmBuzzerIsSounding = false;
static unsigned long inAlarmDuration       = 0;
static unsigned long firstAlarmCallTime    = 0;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Perform short alarm buzzer beep to ensure its working
void performAlarmBuzzerStartupBeep() {
  DEBUG_GENERAL("Performing alarm buzzer startup beep ...");
  tone(alarmBuzzerPin, ALARM_BUZZER_FREQUENCY_HZ, 1500); // 4kHz for 1.5 seconds
}

// Disable the buzzer
void alarmBuzzerDisable() {
  // Return quickly if alarm is not sounding
  if (!alarmBuzzerIsSounding) {
    return;
  }
  noTone(alarmBuzzerPin);
  firstAlarmCallTime    = 0;
  alarmBuzzerIsSounding = false;
  DEBUG_ERROR("Alarm buzzer disabled for critical condition, in alarm duration was: %lu ms", inAlarmDuration);
}

// Sound the buzzer if the engine is running, and after a delay
void alarmBuzzerEnable() {
  if (alarmBuzzerIsSounding) {
    return; // Return quickly if the buzzer is already sounding
  }

  if (firstAlarmCallTime == 0) {
    firstAlarmCallTime = millis();
  }

  inAlarmDuration = millis() - firstAlarmCallTime;

  if (inAlarmDuration > ALARM_BUZZER_ERROR_DELAY_MS && !alarmBuzzerIsSounding) {
    tone(alarmBuzzerPin, ALARM_BUZZER_FREQUENCY_HZ); // 4kHz tone
    alarmBuzzerIsSounding = true;
    DEBUG_ERROR("Alarm buzzer enabled for critical condition, in alarm duration is: %lu ms", inAlarmDuration);
  }
}
