#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "nvs_flash.h"

#include <netdb.h>
#include <sys/socket.h>

#include "dnsserver.h"
#include "webserver.h"

#include "driver/uart.h"
#include "esp_netif.h"

#define WIFI_AP_SSID "自动弹出页面"

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

#define WIFI_CONNECTED_BIT 1

static const char* TAG = "Main";

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    switch (event_id)
    {
        case WIFI_EVENT_AP_STACONNECTED:
        {
            ESP_LOGI(TAG, "a phone connected!");
            break;
        }
        case WIFI_EVENT_AP_STADISCONNECTED:
        {
            ESP_LOGI(TAG, "a phone disconnected ...");
            break;
        }

        default: break;
    }
}

static void initialise_wifi(void)
{
    esp_netif_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    wifi_config_t wifi_ap_config = {
        .ap = {
               .ssid           = WIFI_AP_SSID,
               .ssid_len       = strlen(WIFI_AP_SSID),
               .authmode       = WIFI_AUTH_OPEN,
               .max_connection = 3,
               },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    ESP_LOGI(TAG, "Setting WiFi softAP SSID %s...", wifi_ap_config.ap.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    initialise_wifi();  // init wifi

    dns_server_start();  // run DNS server
    web_server_start();  // run http server
}
