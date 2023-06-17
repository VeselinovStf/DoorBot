#include "index.h"

#include <Arduino.h>
#include "log.h"

#pragma region DEBUGER

void debugger(String message)
{
    if (1)
    {
        Serial.println(message);
    }
}

#pragma endregion DEBUGER