#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_random.h"
#include "nvs_flash.h"

/* https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp_now.html */

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR     "%02X:%02X:%02X:%02X:%02X:%02X"

void delay_ms(uint32_t n)
{
    vTaskDelay(n / portTICK_PERIOD_MS);
}

// custom message
typedef struct {
    char str[32];
    int  num;
} espnow_msg;

// espnow callback (send & recv)
void espnow_on_data_send(const uint8_t* mac_addr, esp_now_send_status_t status)
{
    printf(status == ESP_NOW_SEND_SUCCESS ? "send success\n\r" : "send fail\n\r");
}

void espnow_on_data_recv(const esp_now_recv_info* info, const uint8_t* data, int data_len)
{
    espnow_msg* msg = (espnow_msg*)data;
    printf(MACSTR, MAC2STR(info->src_addr));
    printf(" :%s, %d\n\r", msg->str, msg->num);
}

// all mac
#define dest_mac                           \
    {                                      \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF \
    }

#define key              \
    {                    \
        0x11, 0x22, 0x33 \
    }

// init espnow
void espnow_init(uint8_t mode /* 1:matser,2:slave*/)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES  //
        || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        nvs_flash_init();
    }

    const wifi_init_config_t wifi_cfg =
        WIFI_INIT_CONFIG_DEFAULT();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_wifi_init(&wifi_cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    esp_now_init();
    esp_now_set_pmk((const uint8_t*)"pmk9876543210123");
    if (mode & 1)
    {  // matser
        esp_now_register_recv_cb(espnow_on_data_recv);
    }
    if (mode & 2)
    {  // slave
        esp_now_register_send_cb(espnow_on_data_send);
        const esp_now_peer_info_t broadcast = {
            .peer_addr = dest_mac,
            .lmk       = key,
            .channel   = 0,
            .ifidx     = WIFI_IF_STA,
            .encrypt   = false,
            .priv      = nullptr,
        };
        esp_now_add_peer(&broadcast);
    }
}

extern "C" void app_main(void)
{
#if 1  // rx
    espnow_init(1);
    while (1)
    {
        delay_ms(3000);
    }
#else  // tx
    espnow_init(2);
    const uint8_t peer[] = dest_mac;
    while (1)
    {
        espnow_msg msg = {"str", (int32_t)esp_random()};
        esp_now_send(peer, (uint8_t*)&msg, sizeof(espnow_msg));
        delay_ms(3000);
    }
#endif
}
