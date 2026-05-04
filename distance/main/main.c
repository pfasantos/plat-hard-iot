#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include <stdio.h>

// TAGS
#define GPIO_TAG "gpio_conf"
#define TRIG_TAG "trigger"
#define MAIN_TAG "main"

// pins
#define TRIG_PIN GPIO_NUM_12
#define ECHO_PIN GPIO_NUM_14

#define MAX_DISTANCE_CM 300

volatile int flag =0;

void pin_configurations(void) {
  gpio_config_t echo = {.pin_bit_mask = (1ULL << ECHO_PIN),
                        .mode = GPIO_MODE_INPUT,
                        .pull_down_en = 1,
                        .pull_up_en = 0,
                        .intr_type = GPIO_INTR_ANYEDGE};

  gpio_config(&echo);
  gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(TRIG_PIN, 0);
  ESP_LOGI(GPIO_TAG, "GPIO pins configured\n");
}

void send_trigger(void) {
  int64_t start_time;
  int64_t now = 0;
  int64_t interval = 10;

  // sends 10us high level signal
  start_time = esp_timer_get_time();
  gpio_set_level(TRIG_PIN, 1);
  while (now - start_time <= interval) {
    now = esp_timer_get_time();
  }
  gpio_set_level(TRIG_PIN, 0);
  ESP_LOGI(TRIG_TAG, "Trigger sent\n");
}

static void gpio_isr_handler(void *) {
  flag = gpio_get_level(ECHO_PIN);
}

void app_main(void) {

    int timeok = 1, trigg = 1, ite = 1;
  int64_t range =0, high_time = 0, start_time =0, end_time = 0;
  pin_configurations();

  gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
  gpio_isr_handler_add(ECHO_PIN, gpio_isr_handler, NULL);

  while (1) {
    if (trigg || (ite % 20000000 == 0)) {
      send_trigger();
      trigg = 0;
      ite = 1;
    }

    //printf("FLAG: %d\nTIME: %d\n", flag, timeok);

    if (flag && timeok) {
      start_time = esp_timer_get_time();
      timeok =0;
    }
    else if (!flag && !timeok) {
      end_time = esp_timer_get_time();
      high_time = (end_time - start_time);
      range = high_time / 58.0;
      ESP_LOGI(MAIN_TAG, "range :%lld \n", range);
      
      timeok = 1;
      trigg = 1;
      vTaskDelay(pdMS_TO_TICKS(500));

      ite++;
    }

      ite++;
      //vTaskDelay(pdMS_TO_TICKS(500));
  }
}
