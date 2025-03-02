#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void delay_ms(uint32_t n)
{
    vTaskDelay(n / portTICK_PERIOD_MS);
}

extern "C" void app_main(void)
{
    while (1)
    {
        printf("hello world\r\n");
        delay_ms(1000);
    }
}
