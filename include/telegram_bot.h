#ifndef TELEGRAM_BOT_H
#define TELEGRAM_BOT_H

#include "Arduino.h"

class TelegramBOT
{
public:
    TelegramBOT(char* id,char* api_key, char* wifi_ssid, char* wifi_password);
    void begin(char* message);
    void destroy();
private:
    char* _id;
    char* _api_key;
    char* _wifi_ssid;
    char* _wifi_password;
};
#endif