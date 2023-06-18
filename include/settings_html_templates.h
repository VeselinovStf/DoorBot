#ifndef SETTINGS_HTML_TEMPLATES_H
#define SETTINGS_HTML_TEMPLATES_H

#include <Arduino.h>

class SettingsHTMLTemplate
{
public:
    static const char *PARAM_SSID;
    static const char *PARAM_WIFI_PASS;
    static const char *PARAM_API_KEY;
    static const char *PARAM_ID;
    static const String PARAM_INPUT_MAC;
    static const char *PARAM_STATION_ALARM;
    static const char *PARAM_FILTERING;
    static const char *PARAM_NOTIFICATIONS;
};

#endif