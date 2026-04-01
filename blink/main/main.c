#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

#define LED_PIN GPIO_NUM_2

void app_main(void)
{
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    while (1)
    {
        gpio_set_level(LED_PIN, 0);
        ESP_LOGI("a", "on");
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(LED_PIN, 1);
        ESP_LOGI("a", "off");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}