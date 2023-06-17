#include "index.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "magnetic_sensor.h"
#include "log.h"

#pragma region INDEX_HTTP_RESPONSES

bool indexAlarmService(String currentLine)
{
  if (currentLine.indexOf("GET /AA") >= 0)
  {
    debugger("Alarm Enabled");
    return true;
  }
  if (currentLine.indexOf("GET /DA") >= 0)
  {
    debugger("Alarm Dissabled!");
    return false;
  }
}

bool indexBuzzerService(String currentLine)
{
  if (currentLine.indexOf("GET /BA") >= 0)
  {
    debugger("Buzzer Enabled");
    return true;
  }
  if (currentLine.indexOf("GET /DB") >= 0)
  {
    debugger("Buzzer Dissabled!");
    return false;
  }
}

#pragma endregion INDEX_HTTP_RESPONSES

#pragma region INDEX_PAGE

/*
  Index page View
*/
void indexView(WiFiClient client)
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

#pragma region INDEX_CONTROLLER

/*
  Index Controller
*/
void indexController(WiFiClient client, bool isAlarmOn, bool isBuzzerOn, int BUZZER_PIN, int HALL_SENSOR_UP_LIMIT)
{
  String currentLine = ""; // make a String to hold incoming data from the client
  while (client.connected())
  { // loop while the client's connected
    checkDoorSensor(isAlarmOn, isBuzzerOn, BUZZER_PIN, HALL_SENSOR_UP_LIMIT);

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
          indexView(client);

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
      isAlarmOn = indexAlarmService(currentLine);
      isBuzzerOn = indexBuzzerService(currentLine);
    }

    // Client is not availible
  }
}

#pragma endregion INDEX_CONTROLLER