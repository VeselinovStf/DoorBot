#include "Arduino.h"
#include "credentials.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "credential_html_templates.h"
#include <SPIFFS.h>

const int max_mac_filtering = 20;

CredentialHTMLTemplate credentialHTMLTemplate;

void notFoundCredentials(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

AsyncWebServer credentialServer(80);

credentials_t *credentialModel;

Credentials::Credentials(credentials_t &s)
{
  credentialModel = &s;
}

void Credentials::begin(char *ssid, char *password)
{
 
    WiFi.softAP(ssid, password);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
 

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Send web page with input fields to client
  credentialServer.on("/", HTTP_GET, [&](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/credentials.html", "text/html"); });

 credentialServer.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/bootstrap.min.css", "text/css"); });

   credentialServer.on("/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/bootstrap.min.js", "text/js"); });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  credentialServer.on("/get", HTTP_GET, [&](AsyncWebServerRequest *request)
            {
              String inputMessage;

                  // INPUT_SSID
                  if (request->hasParam(credentialHTMLTemplate.PARAM_SSID))
                  {
                    credentialModel->INPUT_SSID = request->getParam(credentialHTMLTemplate.PARAM_SSID)->value();
                  }

                  // INPUT_PASSWORD
                  if (request->hasParam(credentialHTMLTemplate.PARAM_WIFI_PASS))
                  {
                    credentialModel->INPUT_PASSWORD = request->getParam(credentialHTMLTemplate.PARAM_WIFI_PASS)->value();
                  }
                 
                credentialModel->CLIENT_SETUP_DONE = true; 

              request->send(200, "text/html", "<!DOCTYPE html><html ><head><title>Door Bot v1.0.1</title></head><body><div><h1>Door Bot: Credentials Applied!</h1></div></body></html>");
  
             
             
              });
  credentialServer.onNotFound(notFoundCredentials);
  credentialServer.begin();
}

void Credentials::destroy()
{
  delay(2000);
  WiFi.disconnect();
  credentialServer.end();
}
