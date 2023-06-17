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

#pragma region DEFINE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#define DEBUG 1
#define LED_BUILTIN 2 // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

#define HALL_SENSOR_UP_LIMIT 20
#define BUZZER_PIN 32

// Set these to your desired credentials.
const char *ssid = "AP_0";
const char *password = "12345678";

bool isAlarmOn = true;
bool isBuzzerOn = false;

WiFiServer server(80);

#pragma endregion DEFINE

#pragma region DEBUGER

void debugger(String message)
{
  if (DEBUG)
  {
    Serial.println(message);
  }
}

#pragma endregion DEBUGER

#pragma region SETUP

void setup()
{
  if (DEBUG)
  {
    Serial.begin(115200);
  }

  debugger("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  if (!WiFi.softAP(ssid, password))
  {
    log_e("Soft AP creation failed.");
    while (1)
      ;
  }

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  debugger(myIP.toString());
  server.begin();

  debugger("Server started");

  if (isBuzzerOn)
  {
    pinMode(BUZZER_PIN, OUTPUT);
  }
}

#pragma endregion SETUP

#pragma region DOOR_SENSOR

void checkDoorSensor()
{
  if (isAlarmOn)
  {
    int hallSensorValue = hallRead();
    int buzzerValue = 0;

    if (hallSensorValue >= HALL_SENSOR_UP_LIMIT)
    {
      debugger(F("DOOR IS OPEN: "));
      buzzerValue = 1;
    }
    else
    {
      debugger(F("DOOR IS CLOSED: "));
      buzzerValue = 0;
    }

    if (isBuzzerOn)
    {
      digitalWrite(BUZZER_PIN, buzzerValue);
    }
  }
}

#pragma endregion DOOR_SENSOR

#pragma region HTTP_RESPONSES

void checkResponseForAlarm(String currentLine)
{
  if (currentLine.indexOf("GET /AA") >= 0)
  {
    debugger("Alarm Enabled");
    isAlarmOn = true;
  }
  if (currentLine.indexOf("GET /DA") >= 0)
  {
    debugger("Alarm Dissabled!");
    isAlarmOn = false;
  }
}

void checkResponseForBuzzer(String currentLine)
{
  if (currentLine.indexOf("GET /BA") >= 0)
  {
    debugger("Buzzer Enabled");
    isBuzzerOn = true;
  }
  if (currentLine.indexOf("GET /DB") >= 0)
  {
    debugger("Buzzer Dissabled!");
    isBuzzerOn = false;
  }
}

#pragma endregion HTTP_RESPONSES

#pragma region INDEX_PAGE

void indexPage(WiFiClient client)
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  // MAIN CONTENT
  // the content of the HTTP response follows the header:
  client.print("Click <a href=\"/AA\">here</a> TO ACTIVATE ALARM<br>");
  client.print("Click <a href=\"/DA\">here</a> TO DEACTIVATE ALARM<br>");

  client.print("<hr />");

  // the content of the HTTP response follows the header:
  client.print("Click <a href=\"/BA\">here</a> TO ACTIVATE BUZZER<br>");
  client.print("Click <a href=\"/DB\">here</a> TO DEACTIVATE BUZZER<br>");

  // The HTTP response ends with another blank line:
  client.println();
}

#pragma endregion INDEX_PAGE

void loop()
{
  WiFiClient client = server.available(); // listen for incoming clients
  checkDoorSensor();

  if (client)
  {                          // if you get a client,
    debugger("New Client."); // print a message out the serial port
    String currentLine = ""; // make a String to hold incoming data from the client

    while (client.connected())
    { // loop while the client's connected
      checkDoorSensor();

      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c);        // print it out the serial monitor
        if (c == '\n')
        { // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            indexPage(client);

            // break out of the while loop:
            break;
          }
          else
          { // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }

        // HTTP RESPONSE MESSAGE CHECKS
        checkResponseForAlarm(currentLine);
        checkResponseForBuzzer(currentLine);

      }

      // Client is not availible
    }

    // Client is not connected!
    // Close the connection!
    client.stop();
    debugger("Client Disconnected.");
  }
}
