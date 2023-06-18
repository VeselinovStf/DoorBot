#include "magnetic_sensor.h"
#include <Arduino.h>
#include "log.h"

#pragma region DOOR_SENSOR

bool checkMagneticSensor(int HALL_SENSOR_UP_LIMIT)
{
  int hallSensorValue = hallRead();

  if (hallSensorValue >= HALL_SENSOR_UP_LIMIT)
  {
    debugger(F("Far from magnet. "));
    return false;
  }
  else
  {
    debugger(F("Close to magnet. "));
    return true;
  }
}

#pragma endregion DOOR_SENSOR