set(COMPONENT_SRCS "main.c") 
set(COMPONENT_REQUIRES driver esp_event esp_wifi nvs_flash esp_netif captive)
register_component()
