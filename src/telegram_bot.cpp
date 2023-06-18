#include <Arduino.h>
#include "telegram_bot.h"
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include <WiFi.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "mac_sniffer.h"

WiFiClientSecure client;

TelegramBOT::TelegramBOT(const char *id, const char *api_key, const char *wifi_ssid, const char *wifi_password)
{
    _id = id;
    _api_key = api_key;
    _wifi_ssid = wifi_ssid;
    _wifi_password = wifi_password;
}

void TelegramBOT::begin(char *message)
{
    Serial.println("Telegram Bot initiating!");

    wifi_country_t wifi_country = {.cc = "CN", .schan = 1, .nchan = 13};
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_country(&wifi_country)); /* set country for channel range [1, 13] */
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_start());

    UniversalTelegramBot bot(_api_key, client);

    // Attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    Serial.println(_wifi_ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(_wifi_ssid, _wifi_password);
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Cant connect Telegram Bot!");
        delay(500);
    }

    Serial.println("");
    Serial.println("Telegram BOT WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    bot.sendMessage(_id, message, "");
};

void TelegramBOT::destroy()
{
    delay(200);
    WiFi.disconnect();
    Serial.println("Telegram Bot Destroyed!");
};