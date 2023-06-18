#include "magnetic_sensor.h"
#include <Arduino.h>
#include "log.h"

#pragma region DOOR_SENSOR

bool checkMagneticSensor(int HALL_SENSOR_UP_LIMIT, int UPPER_LIMIT_TRESHHOLD, int DELAY,int HALL_SENSOR_TRESHHOLD_MAX_REPEAT)
{
  int hallSensorValue = hallRead();

  Serial.print("Hall: ");
  Serial.print(hallSensorValue);
  Serial.print(" : ");

  int currentTreshhold = 0;
  int result = 0;
  if (hallSensorValue >= HALL_SENSOR_UP_LIMIT)
  {
    while (currentTreshhold < UPPER_LIMIT_TRESHHOLD)
    {
      delay(DELAY);
      hallSensorValue = hallRead();

      currentTreshhold++;

      if (hallSensorValue >= HALL_SENSOR_UP_LIMIT)
      {
        result++;
      }
    }

    if (result >= HALL_SENSOR_TRESHHOLD_MAX_REPEAT)
    {
      debugger(F("Far from magnet. "));
      return false;
    }

    debugger(F("Don't know the range"));

    return true;
  }
  else
  {
    debugger(F("Close to magnet. "));
    return true;
  }
}

#pragma endregion DOOR_SENSOR