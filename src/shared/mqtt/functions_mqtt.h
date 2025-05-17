#pragma once

#include <Arduino.h>

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */
void connectMqttClientToBroker();
void publishMqttMetric(String, String, int);
void publishMqttMetric(String, String, String);
