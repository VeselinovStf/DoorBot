#ifndef MAC_SNIFFER_H
#define MAC_SNIFFER_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

typedef struct
{
    unsigned frame_ctrl : 16;
    unsigned duration_id : 16;
    uint8_t addr1[6]; /* receiver address */
    uint8_t addr2[6]; /* sender address */
    uint8_t addr3[6]; /* filtering address */
    unsigned sequence_ctrl : 16;
    uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct
{
    wifi_ieee80211_mac_hdr_t hdr;
    uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;

class MacSniffer
{
public:
    void begin();
    void reinit();
    void destroy();
    uint8_t level = 0;
    uint8_t channel = 1;
    static bool foundDevice;
    static int totalKnown;
    static String MAC_FILTER[20];
private:
    static esp_err_t event_handler(void *ctx, system_event_t *event);
    static void wifi_sniffer_init(void);
    static void wifi_sniffer_set_channel(uint8_t channel);
    static const char *wifi_sniffer_packet_type2str(wifi_promiscuous_pkt_type_t type);
    static void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type);
    static int foundDeviceId;
    static void wifi_sniffer_re_init(void);

};

#endif