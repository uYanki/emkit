#include <Arduino.h>
#include <freertos/task.h>
#include <WiFi.h>
#include <time.h>
#include <mini_shell.h>

#define LED_BUILTIN 2

#if CONFIG_FREERTOS_UNICODE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif
#define NTP "ntp.aliyun.com"
const char* ssid     = "HUAWEI-Y6AZGD";
const char* password = "13631520360";

void shell_putc(char ch)
{
    Serial.print(ch);
}

int shell_getc(char* ch)
{
    if (Serial.available())
    {
        *ch = Serial.read();
        return 1;
    }
    return 0;
}

int time_cb(const struct _shell_cmd* pCmdt, int argc, char* const* argv)
{
    struct tm timeInfo;

    if (!getLocalTime(&timeInfo))
    {
        shell_puts("Fail to obtain time\r\n");
        return -1;
    }

    shell_printf("%04d-%02d-%02d %02d:%02d:%02d\r\n", timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday, timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);

    return 1;
}

int add_cb(const struct _shell_cmd* pCmdt, int argc, char* const* argv)
{
    if (argc != 3)
    {
        return 0;
    }

    int a = atoi(argv[1]);
    int b = atoi(argv[2]);

    shell_printf("%d\r\n", a + b);

    return 1;
}

TaskFunction_t toggle = [](void* parameter) {
    while (1)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
};

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    {
        // connect wifi then get curret time from NTP

        WiFi.begin(ssid, password);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print('.');
        }

        Serial.println("connected");

        configTime(8 * 3600, 0, NTP);
        struct tm timeInfo;
        if (!getLocalTime(&timeInfo))
        {
            Serial.println("Fail to obtain time\r\n");
        }
        Serial.println(&timeInfo, "Login at %A, %B %d %Y %H:%M:%S");

        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    }

    shell_add_cmd({.name     = "time",
                   .help     = "show system current time",
                   .callback = time_cb});
    shell_add_cmd({.name     = "add",
                   .help     = "add number",
                   .callback = add_cb});

    xTaskCreatePinnedToCore(toggle, "Toggle", 1024, NULL, 1, NULL, app_cpu);
    xTaskCreatePinnedToCore(mini_shell_task, "shell", 2048, NULL, 2, NULL, app_cpu);
}

void loop()
{
    // put your main code here, to run repeatedly:
}