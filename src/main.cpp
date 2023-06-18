#include <Arduino.h>

#include "credentials.h"
#include "credential_html_templates.h"

#include "settings.h"
#include "settings_html_templates.h"

#include "magnetic_sensor.h"
#include "telegram_bot.h"

credentials_t botCredentials;
Credentials botCredentialsServer(botCredentials);

setting_t botSettings;
Setting botSettingsServer(botSettings);

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

bool destroyBotCredentialServer = false;
bool initiateSettingsServer = false;

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
          // Buzzer
          if (botSettings.STATION_SOUND)
          {
            digitalWrite(BUZZER_PIN, HIGH);
          }

          // Messages
          if (botSettings.NOTIFICATIONS)
          {
            TelegramBOT bot = TelegramBOT(botSettings.INPUT_ID.c_str(), botSettings.INPUT_API_KEY.c_str(), botSettings.INPUT_SSID.c_str(), botSettings.INPUT_PASSWORD.c_str());
            delay(1000);
            
            bot.begin("Door is Open: !!!");
            bot.destroy();

            // Delay for the next notification
            delay(NOTIFICATIONS_SEND_DELAY);

            initiateSettingsServer = false;
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