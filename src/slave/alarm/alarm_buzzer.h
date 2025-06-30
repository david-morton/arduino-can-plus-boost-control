#pragma once

/* ======================================================================
   DEFINES
   ====================================================================== */

#define ALARM_BUZZER_ERROR_DELAY_MS 1000           // Delay before the buzzer starts sounding in milliseconds
#define ALARM_BUZZER_FREQUENCY_HZ 100              // Frequency of the buzzer sound in Hertz
#define ALARM_BUZZER_WARNING_ON_DURATION_MS 250    // Duration of the warning buzzer sound in milliseconds
#define ALARM_BUZZER_WARNING_OFF_DURATION_MS 10000 // Duration between beeps for the warning buzzer in milliseconds

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern bool alarmBuzzerWarningIsSounding;
extern bool alarmBuzzerCriticalIsSounding;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void alarmBuzzerCriticalEnable();
void alarmBuzzerDisable();
void alarmBuzzerWarningEnable();
void performAlarmBuzzerStartupBeep();
