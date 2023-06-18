#include <Arduino.h>

#include "credentials.h"
#include "credential_html_templates.h"

#include "settings.h"
#include "settings_html_templates.h"

#include "magnetic_sensor.h"
#include "telegram_bot.h"

#include "mac_sniffer.h"

credentials_t botCredentials;
Credentials botCredentialsServer(botCredentials);

setting_t botSettings;
Setting botSettingsServer(botSettings);

MacSniffer macSniffer;

// REPLACE WITH YOUR NETWORK CREDENTIALS
char *INITIAL_CREDENTIAL_CLIENT_SSID = "AP_01";
char *INITIAL_CREDENTIAL_CLIENT_PASSWORD = "12345678";

void setup()
{
  Serial.begin(115200);
  delay(10);
  // put your setup code here, to run once:
  botCredentialsServer.begin(INITIAL_CREDENTIAL_CLIENT_SSID, INITIAL_CREDENTIAL_CLIENT_PASSWORD);
}

#define HALL_SENSOR_UP_LIMIT 20
#define BUZZER_PIN 32
#define NOTIFICATIONS_SEND_DELAY 10000
#define MAC_FILTERING_DELAY 10000

bool destroyBotCredentialServer = false;
bool initiateSettingsServer = false;
bool initiateMacSniffer = false;

void sendNotification()
{
  if (botSettings.NOTIFICATIONS)
  {
    TelegramBOT bot = TelegramBOT(botSettings.INPUT_ID.c_str(), botSettings.INPUT_API_KEY.c_str(), botCredentials.INPUT_SSID.c_str(), botCredentials.INPUT_PASSWORD.c_str());
    delay(1000);

    bot.begin("Door is Open: !!!");
    bot.destroy();

    // Delay for the next notification
    delay(NOTIFICATIONS_SEND_DELAY);

    initiateSettingsServer = false;
  }
}

void loop()
{
  if (botCredentials.CLIENT_SETUP_DONE)
  {
    if (!destroyBotCredentialServer)
    {
      botCredentialsServer.destroy();
      destroyBotCredentialServer = true;
      Serial.println(botCredentials.INPUT_SSID);
      Serial.println("AP Done ..");
    }
    else
    {
      if (!initiateSettingsServer) // Replace with Singleton!
      {
        botSettingsServer.begin(botCredentials.INPUT_SSID.c_str(), botCredentials.INPUT_PASSWORD.c_str());
        initiateSettingsServer = true;
      }

      if (botSettings.STATION_ALARM)
      {
        // Open magnetic trigger
        if (!checkMagneticSensor(HALL_SENSOR_UP_LIMIT))
        {

          if (botSettings.MAC_FILTERING)
          {
            botSettingsServer.destroy();

            macSniffer.totalKnown = 1;                        // Total Mac count
            macSniffer.MAC_FILTER[0] = {"00:c3:0a:6c:17:a1"}; // All Macs from list

            if (!initiateMacSniffer)
            {
              macSniffer.begin();
              // initiateMacSniffer = true;
            }
            else
            {
              macSniffer.reinit();
            }

            long mac_snifer_start_time = millis();
            long mac_sniffer_end_time = mac_snifer_start_time;
            while ((mac_sniffer_end_time - mac_snifer_start_time) <= MAC_FILTERING_DELAY) // do this loop for up to 1000mS
            {
              mac_sniffer_end_time = millis();
              if (macSniffer.foundDevice)
              {
                break;
              }
            }

            // code here
            if (macSniffer.foundDevice)
            {
              Serial.println("Device Found");

              macSniffer.destroy();
              initiateSettingsServer = false;
            }
            else
            {
              Serial.println("Device Not Found");

              macSniffer.destroy();
              initiateSettingsServer = false;
              // Messages
              sendNotification();

              // Buzzer
              if (botSettings.STATION_SOUND)
              {
                digitalWrite(BUZZER_PIN, HIGH);
              }
            }
          }
          else
          {
            // Buzzer
            if (botSettings.STATION_SOUND)
            {
              digitalWrite(BUZZER_PIN, HIGH);
            }

            // Messages
            sendNotification();
          }
        }
      }
      else
      {
        // Close magnetic trigger

        // Buzzer
        if (botSettings.STATION_SOUND)
        {
          digitalWrite(BUZZER_PIN, LOW);
        }
      }
    }
  }
}