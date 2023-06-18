#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#include "Arduino.h"

typedef struct
{
    String INPUT_SSID = "";
    String INPUT_PASSWORD = "";
    bool CLIENT_SETUP_DONE = false;
} credentials_t;

class Credentials
{
public:
    Credentials(credentials_t &s);
    void begin(char *ssid, char *password);
    void destroy();
    credentials_t *credentialModel;
};
#endif