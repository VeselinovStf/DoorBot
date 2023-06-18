#ifndef TELEGRAM_BOT_H
#define TELEGRAM_BOT_H

#include "Arduino.h"

class TelegramBOT
{
public:
    TelegramBOT(const char* id,const char* api_key,const char* wifi_ssid,const char* wifi_password);
    void begin(char* message);
    void destroy();
private:
    const char* _id;
    const char* _api_key;
    const char* _wifi_ssid;
    const char* _wifi_password;
};
#endif