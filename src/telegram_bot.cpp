#include <Arduino.h>
#include "telegram_bot.h"
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

WiFiClientSecure client;

TelegramBOT::TelegramBOT(char *id, char *api_key, char *wifi_ssid, char *wifi_password)
{
    _id = id;
    _api_key = api_key;
    _wifi_ssid = wifi_ssid;
    _wifi_password = wifi_password;
}

void TelegramBOT::begin(char* message)
{
    UniversalTelegramBot bot(_api_key, client);

    // Attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    Serial.println(_wifi_ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(_wifi_ssid, _wifi_password);
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    bot.sendMessage(_id, message, "");
};

void TelegramBOT::destroy()
{
    delay(2000);
    WiFi.disconnect();
};