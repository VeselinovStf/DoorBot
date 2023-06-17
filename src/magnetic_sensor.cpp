#include "magnetic_sensor.h"
#include <Arduino.h>
#include "log.h"

#pragma region DOOR_SENSOR

void checkDoorSensor(bool isAlarmOn,bool isBuzzerOn,int BUZZER_PIN, int HALL_SENSOR_UP_LIMIT)
{
  if (isAlarmOn)
  {
    int hallSensorValue = hallRead();
    int buzzerValue = 0;

    if (hallSensorValue >= HALL_SENSOR_UP_LIMIT)
    {
      //debugger(F("DOOR IS OPEN: "));
      buzzerValue = 1;
    }
    else
    {
      //debugger(F("DOOR IS CLOSED: "));
      buzzerValue = 0;
    }

    if (isBuzzerOn)
    {
      digitalWrite(BUZZER_PIN, buzzerValue);
    }
  }
}

#pragma endregion DOOR_SENSOR