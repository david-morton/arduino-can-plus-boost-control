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

bool                 alarmBuzzerWarningIsSounding  = false;
bool                 alarmBuzzerCriticalIsSounding = false;
static unsigned long inAlarmDuration               = 0;
static unsigned long firstAlarmCallTimeCritical    = 0;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Perform short alarm buzzer beep to ensure its working
void performAlarmBuzzerStartupBeep() {
  DEBUG_GENERAL("Performing alarm buzzer startup beep ...");
  tone(alarmBuzzerPin, ALARM_BUZZER_FREQUENCY_HZ, 1500);
}

// Disable the buzzer
void alarmBuzzerDisable() {
  // Return quickly if alarm is not sounding
  if (!alarmBuzzerCriticalIsSounding && !alarmBuzzerWarningIsSounding) {
    return;
  }
  noTone(alarmBuzzerPin);
  firstAlarmCallTimeCritical    = 0;
  alarmBuzzerCriticalIsSounding = false;
  alarmBuzzerWarningIsSounding  = false;
  DEBUG_ERROR("Alarm buzzer disabled, in alarm duration was: %lu ms", inAlarmDuration);
}

// Sound the critical buzzer after a delay
void alarmBuzzerCriticalEnable() {
  if (alarmBuzzerCriticalIsSounding) {
    return; // Return quickly if the buzzer is already sounding
  }

  if (firstAlarmCallTimeCritical == 0) {
    firstAlarmCallTimeCritical = millis();
  }

  inAlarmDuration = millis() - firstAlarmCallTimeCritical;

  if (inAlarmDuration > ALARM_BUZZER_ERROR_DELAY_MS && !alarmBuzzerCriticalIsSounding) {
    tone(alarmBuzzerPin, ALARM_BUZZER_FREQUENCY_HZ); // 4kHz tone
    alarmBuzzerCriticalIsSounding = true;
    DEBUG_ERROR("Alarm buzzer enabled for critical condition, in alarm duration is: %lu ms", inAlarmDuration);
  }
}

// Sound the warning buzzer
static unsigned long firstAlarmCallTimeWarning;
static unsigned long lastAlarmCallTimeWarning;

void alarmBuzzerWarningEnable() {
  if (!alarmBuzzerWarningIsSounding) {
    tone(alarmBuzzerPin, ALARM_BUZZER_FREQUENCY_HZ, ALARM_BUZZER_WARNING_ON_DURATION_MS);
    alarmBuzzerWarningIsSounding = true;
    firstAlarmCallTimeWarning    = millis();
    lastAlarmCallTimeWarning     = millis();
    DEBUG_ERROR("Alarm buzzer enabled for warning condition");
    return;
  }

  if (alarmBuzzerWarningIsSounding && millis() > (lastAlarmCallTimeWarning + ALARM_BUZZER_WARNING_OFF_DURATION_MS)) {
    tone(alarmBuzzerPin, ALARM_BUZZER_FREQUENCY_HZ, ALARM_BUZZER_WARNING_ON_DURATION_MS);
    lastAlarmCallTimeWarning = millis();
    DEBUG_ERROR("Alarm buzzer still sounding for warning condition, in alarm duration is: %lu ms",
                millis() - firstAlarmCallTimeWarning);
  }
}
