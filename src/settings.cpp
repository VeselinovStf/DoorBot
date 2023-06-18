#include "Arduino.h"
#include "settings.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "settings_html_templates.h"
#include <SPIFFS.h>

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "mac_sniffer.h"

const int max_mac_filtering = 20;

SettingsHTMLTemplate settingHTMLTemplate;

void notFoundSettings(AsyncWebServerRequest *request)
{
   request->send(SPIFFS, "/not-found.html", "text/html");
}

AsyncWebServer settingsServer(80);

setting_t *settingModel;

Setting::Setting(setting_t &s)
{
  settingModel = &s;
}

void Setting::begin(const char *ssid, const char *password)
{
  WiFi.disconnect();
  Serial.println("Settings Server is Starting...");

  wifi_country_t wifi_country = {.cc = "CN", .schan = 1, .nchan = 13};
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_country(&wifi_country)); /* set country for channel range [1, 13] */
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Settings Server WiFi Failed!");

    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Cant connect Settings Server!");
      delay(500);
    }
  }
  else
  {
    Serial.println("Settings Server Working!");
  }

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  else
  {
    Serial.println("Settings server Pages are served!");
  }

  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  settingsServer.on("/", HTTP_GET, [&](AsyncWebServerRequest *request)
                    { request->send(SPIFFS, "/settings.html", "text/html"); });

  settingsServer.on("/magnet", HTTP_GET, [&](AsyncWebServerRequest *request)
                    { request->send(SPIFFS, "/magnetic.html", "text/html"); });

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

              request->send(SPIFFS,"/change-success.html", "text/html");
  
             
              settingModel->CLIENT_SETUP_DONE = true; });

  settingsServer.onNotFound(notFoundSettings);
  settingsServer.begin();
}

void Setting::destroy()
{
  delay(2000);
  ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false)); // set as 'false' the promiscuous mode
  ESP_ERROR_CHECK(esp_wifi_stop());                 // it stop soft-AP and free soft-AP control block
  ESP_ERROR_CHECK(esp_wifi_deinit());               // free all resource allocated in esp_wifi_init() and stop WiFi task
  settingsServer.end();
  Serial.println("Settings Server Destroyed!");
}
