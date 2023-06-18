#include <Arduino.h>

#include <WiFi.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "mac_sniffer.h"

bool MacSniffer::foundDevice = 0;
int MacSniffer::foundDeviceId = 0;
int MacSniffer::totalKnown = 0;
String MacSniffer::MAC_FILTER[20];

esp_err_t MacSniffer::event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

void MacSniffer::wifi_sniffer_init(void)
{
    wifi_country_t wifi_country = {.cc = "CN", .schan = 1, .nchan = 13};

    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(MacSniffer::event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_country(&wifi_country)); /* set country for channel range [1, 13] */
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
}

void MacSniffer::wifi_sniffer_re_init(void)
{
    wifi_country_t wifi_country = {.cc = "CN", .schan = 1, .nchan = 13};

    nvs_flash_init();
    tcpip_adapter_init();
    //ESP_ERROR_CHECK(esp_event_loop_init(MacSniffer::event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_country(&wifi_country)); /* set country for channel range [1, 13] */
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
}

void MacSniffer::wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type)
{
    if (type != WIFI_PKT_MGMT)
        return;

    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

    char buffer[25];
    snprintf(buffer, sizeof(buffer), "%02x:%02x:%02x:%02x:%02x:%02x", hdr->addr1[0], hdr->addr1[1], hdr->addr1[2],
             hdr->addr1[3], hdr->addr1[4], hdr->addr1[5]);

    char bufferTwo[25];
    snprintf(bufferTwo, sizeof(buffer), "%02x:%02x:%02x:%02x:%02x:%02x", hdr->addr2[0], hdr->addr2[1], hdr->addr2[2],
             hdr->addr2[3], hdr->addr2[4], hdr->addr2[5]);

    char bufferThree[25];
    snprintf(bufferThree, sizeof(buffer), "%02x:%02x:%02x:%02x:%02x:%02x", hdr->addr3[0], hdr->addr3[1], hdr->addr3[2],
             hdr->addr3[3], hdr->addr3[4], hdr->addr3[5]);

    for (int i = 0; i < totalKnown; i++)
    {
        if (MAC_FILTER[i] == String(buffer))
        {
            foundDevice = true;
            foundDeviceId = i;
            Serial.println("FOUND ADD1");
            return;
        }
        else if (MAC_FILTER[i] == String(bufferTwo))
        {
            foundDevice = true;
            foundDeviceId = i;
            Serial.println("FOUND ADD2");
            return;
        }
        else if (MAC_FILTER[i] == String(bufferThree))
        {
            foundDevice = true;
            foundDeviceId = i;
            Serial.println("FOUND ADD3");
            return;
        }
        else
        {
            printf(
                " ADDR1=%02x:%02x:%02x:%02x:%02x:%02x,"
                " ADDR2=%02x:%02x:%02x:%02x:%02x:%02x,"
                " ADDR3=%02x:%02x:%02x:%02x:%02x:%02x\n",
                /* ADDR1 */
                hdr->addr1[0], hdr->addr1[1], hdr->addr1[2],
                hdr->addr1[3], hdr->addr1[4], hdr->addr1[5],
                /* ADDR2 */
                hdr->addr2[0], hdr->addr2[1], hdr->addr2[2],
                hdr->addr2[3], hdr->addr2[4], hdr->addr2[5],
                /* ADDR3 */
                hdr->addr3[0], hdr->addr3[1], hdr->addr3[2],
                hdr->addr3[3], hdr->addr3[4], hdr->addr3[5]);
        }
    }
}

void MacSniffer::wifi_sniffer_set_channel(uint8_t channel)
{
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

void MacSniffer::begin()
{
    Serial.println("Mac Sniffer is Starting...");

    wifi_sniffer_init();
}

void MacSniffer::reinit()
{
    Serial.println("Mac Sniffer is Restarting...");

    wifi_sniffer_re_init();
}

void MacSniffer::destroy()
{
    foundDevice = false;
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false)); // set as 'false' the promiscuous mode
    ESP_ERROR_CHECK(esp_wifi_stop());                 // it stop soft-AP and free soft-AP control block
    ESP_ERROR_CHECK(esp_wifi_deinit());               // free all resource allocated in esp_wifi_init() and stop WiFi task
    delay(2000);
    Serial.println("Destroying Mac Sniff!!");
}