#include "Arduino.h"
#include "settings.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "settings_html_templates.h"
#include <SPIFFS.h>

const int max_mac_filtering = 20;

SettingsHTMLTemplate settingHTMLTemplate;

void notFoundSettings(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

AsyncWebServer settingsServer(80);

setting_t *settingModel;

Setting::Setting(setting_t &s)
{
  settingModel = &s;
}

void Setting::begin(const char *ssid, const char *password)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("WiFi Failed!");
    return;
  }

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  settingsServer.on("/", HTTP_GET, [&](AsyncWebServerRequest *request)
                    { request->send(SPIFFS, "/settings.html", "text/html"); });

  settingsServer.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
                    { request->send(SPIFFS, "/bootstrap.min.css", "text/css"); });

  settingsServer.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
                    { request->send(SPIFFS, "/style.css", "text/css"); });

  settingsServer.on("/back.css", HTTP_GET, [](AsyncWebServerRequest *request)
                    { request->send(SPIFFS, "/back.css", "text/css"); });

  settingsServer.on("/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
                    { request->send(SPIFFS, "/bootstrap.min.js", "text/js"); });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  settingsServer.on("/get", HTTP_GET, [&](AsyncWebServerRequest *request)
                    {
              String inputMessage;

              // GET station alarm
              if (request->hasParam(settingHTMLTemplate.PARAM_STATION_ALARM))
              {
                inputMessage = request->getParam(settingHTMLTemplate.PARAM_STATION_ALARM)->value();
                if (inputMessage == "1")
                {
                  settingModel->STATION_ALARM = true;
                }
                else
                {
                  settingModel->STATION_ALARM = false;
                }
              }

              // GET station alarm
              if (request->hasParam(settingHTMLTemplate.PARAM_STATION_SOUND_ALARM))
              {
                inputMessage = request->getParam(settingHTMLTemplate.PARAM_STATION_SOUND_ALARM)->value();
                if (inputMessage == "1")
                {
                  settingModel->STATION_SOUND = true;
                }
                else
                {
                  settingModel->STATION_SOUND = false;
                }
              }

              //GET filtering
              if (request->hasParam(settingHTMLTemplate.PARAM_FILTERING))
              {
                inputMessage = request->getParam(settingHTMLTemplate.PARAM_FILTERING)->value();
                if (inputMessage == "1")
                {
                  settingModel->MAC_FILTERING = true;

                  for (int i = 1; i <= max_mac_filtering; i++)
                  {
                    String current_param = settingHTMLTemplate.PARAM_INPUT_MAC + i;
                    if (request->hasParam(current_param))
                    {
                      settingModel->INPUT_MAC_FILTER[i - 1] = request->getParam(current_param)->value();
                    }
                  }
                }
                else
                {
                  settingModel->MAC_FILTERING = false;
                }
              }

              // GET notifications
              if (request->hasParam(settingHTMLTemplate.PARAM_NOTIFICATIONS))
              {
                inputMessage = request->getParam(settingHTMLTemplate.PARAM_NOTIFICATIONS)->value();
                if (inputMessage == "1")
                {
                  settingModel->NOTIFICATIONS = true;

                  // INPUT_SSID
                  if (request->hasParam(settingHTMLTemplate.PARAM_SSID))
                  {
                    settingModel->INPUT_SSID = request->getParam(settingHTMLTemplate.PARAM_SSID)->value();
                  }

                  // INPUT_PASSWORD
                  if (request->hasParam(settingHTMLTemplate.PARAM_WIFI_PASS))
                  {
                    settingModel->INPUT_PASSWORD = request->getParam(settingHTMLTemplate.PARAM_WIFI_PASS)->value();
                  }

                  //
                  if (request->hasParam(settingHTMLTemplate.PARAM_API_KEY))
                  {
                    settingModel->INPUT_API_KEY = request->getParam(settingHTMLTemplate.PARAM_API_KEY)->value();
                  }

                  //
                  if (request->hasParam(settingHTMLTemplate.PARAM_ID))
                  {
                    settingModel->INPUT_ID = request->getParam(settingHTMLTemplate.PARAM_ID)->value();
                  }
                }
                else
                {
                  settingModel->NOTIFICATIONS = false;
                }
              }

              request->send(200, "text/html", "<!DOCTYPE html><html ><head><title>Door Bot v1.0.1</title></head><body><div><h1>The Door Settings</h1><div><h4>Station Alarm: " + String(settingModel->STATION_ALARM) + "</h4><h4>MAC Addr Filter: " + String(settingModel->MAC_FILTERING) + "</h4><h4>Notifications: " + String(settingModel->NOTIFICATIONS) + "</h4><p>SSID: " + settingModel->INPUT_SSID + "</p></div></body></html>");
  
             
              settingModel->CLIENT_SETUP_DONE = true; });
  settingsServer.onNotFound(notFoundSettings);
  settingsServer.begin();
}

void Setting::destroy()
{
  delay(2000);
  WiFi.disconnect();
  settingsServer.end();
}
