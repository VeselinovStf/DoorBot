#ifndef INDEX_H
#define INDEX_H

#include <Arduino.h>
#include <WiFiClient.h>

bool indexAlarmService(String currentLine);
bool indexBuzzerService(String currentLine);
void indexView(WiFiClient client);
void indexController(WiFiClient client, bool isAlarmOn, bool isBuzzerOn, int BUZZER_PIN, int HALL_SENSOR_UP_LIMIT);
#endif