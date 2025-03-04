
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "rc522.h"

#define LED_GPIO 12

void tag_handler(uint8_t* serial_no)
{
    for (int i = 0; i < 5; i++)
    {
        printf("%#x ", serial_no[i]);
    }
    printf("\n");

    gpio_set_level(LED_GPIO, 0);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    gpio_set_level(LED_GPIO, 1);
}

void app_main(void)
{
    esp_rom_gpio_pad_select_gpio(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    const rc522_start_args_t start_args = {
        .miso_io  = 25,
        .mosi_io  = 23,
        .sck_io   = 19,
        .sda_io   = 22,
        .callback = &tag_handler,
    };

    rc522_start(start_args);
}
