#pragma once

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern bool globalAlarmCriticalState;
extern bool globalAlarmWarningState;
extern bool localAlarmCriticalState;
extern bool localAlarmWarningState;
extern bool remoteAlarmCriticalState;
extern bool remoteAlarmWarningState;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void evaluateGlobalAlarmStates();
