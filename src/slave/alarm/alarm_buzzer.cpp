#include <Arduino.h>

#include "../../shared/alarm/alarm_helpers.h"
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

bool alarmBuzzerWarningIsSounding  = false;
bool alarmBuzzerCriticalIsSounding = false;

static unsigned long firstAlarmCallTimeCritical = 0;
static unsigned long firstAlarmCallTimeWarning  = 0;

static unsigned long inAlarmDurationCritical = 0;
static unsigned long inAlarmDurationWarning  = 0;

static unsigned long lastAlarmCallTimeWarning;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Perform short alarm buzzer beep to ensure its working
void performAlarmBuzzerStartupBeep() {
  DEBUG_GENERAL("Performing alarm buzzer startup beep ...");
  tone(alarmBuzzerPin, ALARM_BUZZER_FREQUENCY_HZ, 1500);
}

// Disable the alarm buzzer
void alarmBuzzerDisable() {
  // Return quickly if alarm is not sounding
  if (!alarmBuzzerCriticalIsSounding && !alarmBuzzerWarningIsSounding) {
    return;
  }

  // Existing critical alarm state
  if (alarmBuzzerCriticalIsSounding && firstAlarmCallTimeCritical != 0) {
    inAlarmDurationCritical = millis() - firstAlarmCallTimeCritical;
    DEBUG_ERROR("Critical alarm buzzer disabled. Critical duration: %lu ms", inAlarmDurationCritical);
  }

  if (alarmBuzzerWarningIsSounding && firstAlarmCallTimeWarning != 0) {
    inAlarmDurationWarning = millis() - firstAlarmCallTimeWarning;
    DEBUG_ERROR("Warning alarm buzzer disabled. Warning duration: %lu ms", inAlarmDurationWarning);
  }

  noTone(alarmBuzzerPin);

  alarmBuzzerCriticalIsSounding = false;
  alarmBuzzerWarningIsSounding  = false;

  firstAlarmCallTimeCritical = 0;
  firstAlarmCallTimeWarning  = 0;
}

// Sound the critical buzzer after a delay
void alarmBuzzerCriticalEnable() {
  if (alarmBuzzerCriticalIsSounding) {
    return;
  }

  if (firstAlarmCallTimeCritical == 0) {
    firstAlarmCallTimeCritical = millis();
  }

  unsigned long inAlarmDuration = millis() - firstAlarmCallTimeCritical;

  if (inAlarmDuration > ALARM_BUZZER_ERROR_DELAY_MS && !alarmBuzzerCriticalIsSounding) {
    tone(alarmBuzzerPin, ALARM_BUZZER_FREQUENCY_HZ);
    alarmBuzzerCriticalIsSounding = true;
    DEBUG_ERROR("Alarm buzzer enabled for critical condition, in alarm duration is: %lu ms", inAlarmDuration);
  }
}

// Sound the warning buzzer
void alarmBuzzerWarningEnable() {
  // Cater for transition from critical to warning state directly
  if (!globalAlarmCriticalState) {
    noTone(alarmBuzzerPin);
    alarmBuzzerCriticalIsSounding = false;
    firstAlarmCallTimeCritical    = 0;
  }

  // Initiate the buzzer warning sound if not already sounding
  if (!alarmBuzzerWarningIsSounding) {
    tone(alarmBuzzerPin, ALARM_BUZZER_FREQUENCY_HZ, ALARM_BUZZER_WARNING_ON_DURATION_MS);
    alarmBuzzerWarningIsSounding = true;
    firstAlarmCallTimeWarning    = millis();
    lastAlarmCallTimeWarning     = millis();
    DEBUG_ERROR("Alarm buzzer enabled for warning condition");
    return;
  }
  // If the buzzer is already sounding, check if it needs to be re-triggered
  if (alarmBuzzerWarningIsSounding && millis() > (lastAlarmCallTimeWarning + ALARM_BUZZER_WARNING_OFF_DURATION_MS)) {
    tone(alarmBuzzerPin, ALARM_BUZZER_FREQUENCY_HZ, ALARM_BUZZER_WARNING_ON_DURATION_MS);
    lastAlarmCallTimeWarning = millis();
    DEBUG_ERROR("Alarm buzzer still sounding for warning condition, in alarm duration is: %lu ms",
                millis() - firstAlarmCallTimeWarning);
  }
}
