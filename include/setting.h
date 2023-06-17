#ifndef SETTING_H
#define SETTING_H

#include <Arduino.h>
#include <WiFiClient.h>

void setupWIFI(bool accessPointAuthenticated, char *ssid, char *password, WiFiServer server);
void accessPointService(String currentLine, char *ssid, char *password);
void settingController(WiFiClient client, char *ssid, char *password, bool isAlarmOn, bool isBuzzerOn, int BUZZER_PIN, int HALL_SENSOR_UP_LIMIT);

#endif