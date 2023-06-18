#ifndef SETTINGS_H
#define SETTINGS_H

#include "Arduino.h"

typedef struct
{
    String INPUT_SSID = "";
    String INPUT_PASSWORD = "";
    String INPUT_API_KEY = "";
    String INPUT_ID = "";
    bool STATION_ALARM = false;
    bool STATION_SOUND = false;
    bool NOTIFICATIONS = false;
    bool MAC_FILTERING = false;
    bool CLIENT_SETUP_DONE = false;
    String INPUT_MAC_FILTER[20];
} setting_t;

class Setting
{
public:
    Setting(setting_t &s);
    void begin(const char *ssid, const char *password);
    void destroy();
    setting_t *settingModel;
};
#endif