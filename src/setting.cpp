#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "setting.h"
#include "magnetic_sensor.h"
#include "log.h"

#pragma region SETUP

/*
  Setups WIFI Connection:
    - if accessPointAuthenticated is done, use provided SSID and PAsSW to connect to AP
    - else if accessPointAuthenticated is false, start AP mode for setting up the credentials
*/
void setupWIFI(bool accessPointAuthenticated, char *ssid,  char* password, WiFiServer* server)
{
    if (!accessPointAuthenticated)
    {
        debugger("AP: Configuring access point for setting the Door Bot...");

        // You can remove the password parameter if you want the AP to be open.
        // a valid password must have more than 7 characters
        if (!WiFi.softAP(ssid, password))
        {
            log_e("Soft AP creation failed.");
            while (1)
                ;
        }

        IPAddress myIP = WiFi.softAPIP();
        debugger("AP IP address: ");
        debugger(myIP.toString());
        server->begin();

        debugger("Server started");
    }
    else
    {
        debugger("STA: Configuring access point...");

        // You can remove the password parameter if you want the AP to be open.
        // a valid password must have more than 7 characters
        WiFi.mode(WIFI_STA);
        if (!WiFi.begin(ssid, password))
        {
            log_e("Can't connect to access point.");
            while (1)
                ;
        }

        IPAddress myIP = WiFi.localIP();
        debugger("IP address: ");
        debugger(myIP.toString());

        server->begin(8088);

        debugger("Server started: localhost:8088");
    }
}

#pragma endregion SETUP

#pragma region SETTING_HTTP_RESPONSES

bool accessPointService(String* currentLine, char *ssid, char *password)
{
    return ssidService(currentLine,ssid) && passwordService(currentLine,password);
}

bool ssidService(String* currentLine, char *ssid)
{
    if (currentLine->indexOf("ssidInput") >= 0)
    {
        debugger("SSID ..");
        ssid = "A1_41C2";

        return true;
    }

    return false;
}

bool passwordService(String* currentLine, char *password)
{
    if (currentLine->indexOf("wifiPasswordInput") >= 0)
    {
        debugger("Password ..");
        password = "48575443E694A49D";
    
        return true;
    }

    return false;
}


#pragma endregion SETTING_HTTP_RESPONSES

#pragma region SETTING_PAGE

/*
  Settings page View
*/
void settingsView(WiFiClient* client)
{
    client->println("HTTP/1.1 200 OK");
    client->println("Content-type:text/html");
    client->println();

    // MAIN CONTENT
    client->print("<form action=\"/get\">");

    client->print("<label for=\"ssid\">SSID</label>");
    client->print("<input id=\"ssid\" type=\"text\" name=\"ssidInput\">");
    client->print(" <label for=\"password\">PASSWORD</label>");
    client->print(" <input id=\"password\" type=\"text\" name=\"wifiPasswordInput\">");

    client->print(" <input type=\"submit\" value=\"Submit\">");

    client->print(" </form>");

    // The HTTP response ends with another blank line:
    client->println();
}

/*
  Settings page Controller
*/
void settingController(WiFiClient* client, char *ssid,  char* password,bool isAlarmOn,bool isBuzzerOn,int BUZZER_PIN, int HALL_SENSOR_UP_LIMIT, bool* status )
{
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client->connected())
    { // loop while the client's connected
        checkDoorSensor(isAlarmOn, isBuzzerOn, BUZZER_PIN, HALL_SENSOR_UP_LIMIT);

        if (client->available())
        {                           // if there's bytes to read from the client,
            char c = client->read(); // read a byte, then
            Serial.write(c);        // print it out the serial monitor
            if (c == '\n')
            { // if the byte is a newline character

                // if the current line is blank, you got two newline characters in a row.
                // that's the end of the client HTTP request, so send a response:
                if (currentLine.length() == 0)
                {
                    settingsView(client);

                    // break out of the while loop:
                    break;
                }
                else
                { // if you got a newline, then clear currentLine:
                    currentLine = "";
                }
            }
            else if (c != '\r')
            {                     // if you got anything else but a carriage return character,
                currentLine += c; // add it to the end of the currentLine
            }

            // HTTP RESPONSE MESSAGE CHECKS
            *status = accessPointService(&currentLine,ssid,password);
        }

        // Client is not availible
    }
}

#pragma endregion SETTING_PAGE
