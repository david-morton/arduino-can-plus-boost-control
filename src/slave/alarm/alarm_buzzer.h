#pragma once

/* ======================================================================
   DEFINES
   ====================================================================== */

#define ALARM_BUZZER_ERROR_DELAY_MS 1000 // Delay before the buzzer starts sounding in milliseconds
#define ALARM_BUZZER_FREQUENCY_HZ 100 // Frequency of the buzzer sound in Hertz

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern bool alarmBuzzerIsSounding;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void alarmBuzzerEnable();
void alarmBuzzerDisable();
void performAlarmBuzzerStartupBeep();
