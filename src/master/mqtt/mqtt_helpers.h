#pragma once

#include <Arduino.h>

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void connectMqttClientToBroker();
void sendMqttMetric(String topic, String value, int qos);
void sendMqttMetric(String topic, String value, String units);
