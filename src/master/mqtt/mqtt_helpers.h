#pragma once

#include <Arduino.h>

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void connectMqttClientToBroker();
void sendMqttMetric(String, String, int);
void sendMqttMetric(String, String, String);
