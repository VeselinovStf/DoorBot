/*
  main.cpp creates a WiFi access point and provides a web server on it, on witch client can setup door bot.

  Steps:
  1. Connect to the access point
  2. Point your web browser to http://192.168.4.1
    * [GET]
      - http://192.168.4.1/AA - Activate door detection
      - http://192.168.4.1/DA - Deactivate door detection
      - http://192.168.4.1/BA - Activate buzzer
      - http://192.168.4.1/DA - Deactivate Buzzer

  Created for arduino-esp32 on 17 Jun, 2023
  by Stefan Veselinov (chofexx)
*/
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#include "index.h"
#include "setting.h"
#include "magnetic_sensor.h"
#include "log.h"

#pragma region DEFINE

#define LED_BUILTIN 2 // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED
#define HALL_SENSOR_UP_LIMIT 20
#define BUZZER_PIN 32
#define PORT 80
#define DEBUG 1

// Set these to your desired credentials.
char *ssid = "AP_0";
char *password = "12345678";
bool accessPointAuthenticated = false;
bool isAlarmOn = true;
bool isBuzzerOn = false;

WiFiServer server(PORT);

#pragma endregion DEFINE

#pragma region SETUP

void setup()
{
  if (DEBUG)
  {
    Serial.begin(115200);
  }

  setupWIFI(accessPointAuthenticated, ssid, password, server);

  if (isBuzzerOn)
  {
    pinMode(BUZZER_PIN, OUTPUT);
  }
}

#pragma endregion SETUP

void loop()
{
  WiFiClient client = server.available(); // listen for incoming clients
  checkDoorSensor(isAlarmOn, isBuzzerOn, BUZZER_PIN, HALL_SENSOR_UP_LIMIT);

  if (client)
  {                          // if you get a client,
    debugger("New Client."); // print a message out the serial port

    if (accessPointAuthenticated)
    {
      indexController(client, isAlarmOn, isBuzzerOn, BUZZER_PIN, HALL_SENSOR_UP_LIMIT);
    }
    else
    {
      settingController(client, ssid, password, isAlarmOn, isBuzzerOn, BUZZER_PIN, HALL_SENSOR_UP_LIMIT);

      WiFi.disconnect();

      accessPointAuthenticated = true;
      setupWIFI(accessPointAuthenticated, ssid, password, server);
    }

    // Client is not connected!
    // Close the connection!
    client.stop();
    debugger("Client Disconnected.");
  }
}
