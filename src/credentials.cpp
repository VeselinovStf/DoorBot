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
     request->send(SPIFFS, "/not-found.html", "text/html");
}

AsyncWebServer credentialServer(80);

credentials_t *credentialModel;

Credentials::Credentials(credentials_t &s)
{
  credentialModel = &s;
}

void Credentials::begin(char *ssid, char *password)
{
  Serial.println("Credential Server is Starting...");
  WiFi.disconnect();

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
  else
  {
    Serial.println("Credential Server is Working!");
  }

  // Send web page with input fields to client
  credentialServer.on("/", HTTP_GET, [&](AsyncWebServerRequest *request)
                      { request->send(SPIFFS, "/credentials.html", "text/html"); });

  credentialServer.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
                      { request->send(SPIFFS, "/bootstrap.min.css", "text/css"); });

  credentialServer.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
                      { request->send(SPIFFS, "/style.css", "text/css"); });

  credentialServer.on("/back.css", HTTP_GET, [](AsyncWebServerRequest *request)
                      { request->send(SPIFFS, "/back.css", "text/css"); });

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

                        request->send(SPIFFS, "/credential-applied.html", "text/html");
                      });
  credentialServer.onNotFound(notFoundCredentials);
  credentialServer.begin();
}

void Credentials::destroy()
{
  delay(2000);
  WiFi.disconnect();
  credentialServer.end();
  Serial.println("Credential Server Destroyed!");
}
