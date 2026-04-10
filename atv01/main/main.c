#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "hal/adc_types.h"
#include "hal/gpio_types.h"
#include "hal/ledc_types.h"
#include "soc/clk_tree_defs.h"
#include <inttypes.h>
#include <stdint.h>

#define POT_CHANNEL ADC_CHANNEL_4 // GPIO 15: ADC channel used by potentiometer.
#define LDR_CHANNEL ADC_CHANNEL_6 // GPIO 17: ADC channel used by LDR.

#define LED0_GPIO 39 /* LED controlled by intensity (PWM) */
#define LED1_GPIO 35 /* LED controlled by time (blink period) */

#define PWM_TIMER LEDC_TIMER_0 // Timer used for PWM configuration.

#define BUTTON 4 /* Pin n° 4 */

void blink(int time) {
  /* Non-busy wait */
  gpio_set_level(LED1_GPIO, 1);
  vTaskDelay(pdMS_TO_TICKS(time));

  gpio_set_level(LED1_GPIO, 0);
  vTaskDelay(pdMS_TO_TICKS(time));
}

void app_main() {

  adc_oneshot_unit_handle_t adc_handle;
  adc_oneshot_unit_init_cfg_t init_config = {.unit_id = ADC_UNIT_2};

  adc_oneshot_new_unit(&init_config, &adc_handle);

  adc_oneshot_chan_cfg_t config = {.bitwidth = ADC_BITWIDTH_12}; /* 0 to 4095 */

  /* Potentiometer and LDR channel configuration. */
  adc_oneshot_config_channel(adc_handle, POT_CHANNEL, &config);
  adc_oneshot_config_channel(adc_handle, LDR_CHANNEL, &config);

  ledc_timer_config_t timer = {
      .speed_mode = LEDC_LOW_SPEED_MODE, // ESP32-S3 only have low speed mode.
      .duty_resolution = LEDC_TIMER_12_BIT, // 4096 values
      .timer_num = PWM_TIMER, /* It should be the same as 'led_N.timer_sel' */
      .freq_hz = 5000,
      .clk_cfg = LEDC_AUTO_CLK,
  };

  ledc_channel_config_t led_0 = {
      .gpio_num = LED0_GPIO,
      .channel = LEDC_CHANNEL_0,
      .timer_sel = PWM_TIMER,
      .duty = 0,   // ?
      .hpoint = 0, // ?
  };

  ledc_channel_config_t led_1 = {
      .gpio_num = LED1_GPIO,
      .channel = LEDC_CHANNEL_1,
      .timer_sel = PWM_TIMER,
      .duty = 0,   // ?
      .hpoint = 0, // ?
  };

  /* Adding led_0 configuration. */
  ledc_timer_config(&timer);
  ledc_channel_config(&led_0);
  ledc_channel_config(&led_1);

  /* Variables used to read ADC's channels. */
  int pot_value;
  int ldr_value;

  /* Button config */
  gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
  gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);

  /* Stores the last state to detect mode changes. */
  int last_input = 1;
  int mode = 1;

  while (1) {
    int actual_input = gpio_get_level(BUTTON);

    adc_oneshot_read(adc_handle, POT_CHANNEL, &pot_value);
    adc_oneshot_read(adc_handle, LDR_CHANNEL, &ldr_value);
    printf("--------------\nBUTTON: %d\nPOT: %d\nLDR: %d\n----------\n",
           actual_input, pot_value, ldr_value);

    printf("MODE: %d\n", mode);
    /* Falling edge detect (BUTTON: HIGH -> LOW && LAST_INPUT == 1) */
    if (!actual_input && last_input)
      mode ^= 1; /* Toggle between POT and LDR mode */

    last_input = actual_input;

    if (mode) { // Pot
      adc_oneshot_read(adc_handle, POT_CHANNEL, &pot_value);

      ledc_set_duty(LEDC_LOW_SPEED_MODE, led_0.channel, pot_value);
      ledc_update_duty(LEDC_LOW_SPEED_MODE, led_0.channel);

      int time = 100 + (pot_value * 900) / 4095;

      ledc_set_duty(LEDC_LOW_SPEED_MODE, led_1.channel, 4095);
      ledc_update_duty(LEDC_LOW_SPEED_MODE, led_1.channel);
      vTaskDelay(pdMS_TO_TICKS(time));

      ledc_set_duty(LEDC_LOW_SPEED_MODE, led_1.channel, 0);
      ledc_update_duty(LEDC_LOW_SPEED_MODE, led_1.channel);
      vTaskDelay(pdMS_TO_TICKS(time));

      //      blink(100 + (pot_value * 900) / 4095);

    } else { // LDR
      adc_oneshot_read(adc_handle, LDR_CHANNEL, &ldr_value);

      ledc_set_duty(LEDC_LOW_SPEED_MODE, led_0.channel, ldr_value);
      ledc_update_duty(LEDC_LOW_SPEED_MODE, led_0.channel);

      int time = 100 + (ldr_value * 900) / 4095;

      ledc_set_duty(LEDC_LOW_SPEED_MODE, led_1.channel, 4095);
      ledc_update_duty(LEDC_LOW_SPEED_MODE, led_1.channel);
      vTaskDelay(pdMS_TO_TICKS(time));

      ledc_set_duty(LEDC_LOW_SPEED_MODE, led_1.channel, 0);
      ledc_update_duty(LEDC_LOW_SPEED_MODE, led_1.channel);
      vTaskDelay(pdMS_TO_TICKS(time));
    }
  }
}
