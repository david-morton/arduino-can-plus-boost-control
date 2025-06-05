#pragma once

#include <Arduino.h>

bool handlePingRequestOrResponse(int cmdId, const char *payload, size_t payloadLength);
