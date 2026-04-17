#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "esp_timer.h"

// TAGS
#define GPIO_TAG "gpio_conf"
#define TRIG_TAG "trigger"
#define MAIN_TAG "main"

//pins
#define TRIG_PIN GPIO_NUM_17
#define ECHO_PIN GPIO_NUM_16

#define MAX_DISTANCE_CM 300

void pin_configurations(void){
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
    gpio_set_level(TRIG_PIN, 0);
    printf("pins confiured\n");
}

void send_trigger(void){
    int64_t start_time;
    int64_t now = 0;
    int64_t interval = 10;
    
    // sends 10us high level signal
    start_time = esp_timer_get_time();
    gpio_set_level(TRIG_PIN, 1);
    while(now - start_time <= interval){
        now = esp_timer_get_time();
    }
    gpio_set_level(TRIG_PIN, 0);
    printf("trigger sent\n");
}

void app_main(void)
{
  int64_t range, high_time, start_time, end_time;
  int echo_level;

  printf("start\n");
  pin_configurations();

  while(1){
    // sends trigger if allowed
    send_trigger();

    echo_level = gpio_get_level(ECHO_PIN);

    if (echo_level) {
      start_time = esp_timer_get_time();

      // wait untill echo level gets low
      while(echo_level) {
          echo_level = gpio_get_level(ECHO_PIN);
      }
      end_time = esp_timer_get_time();
         
      // calculate distance
      high_time = (end_time - start_time);
      range = high_time / 58;

      if (range > MAX_DISTANCE_CM) {
         range = 0;
      }
      printf("distance: %lld cm \n", range);
      vTaskDelay(pdMS_TO_TICKS(2000));
    }
  }
}
