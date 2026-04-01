#include <stdio.h>
#include "driver/gpio.h"
#include  "driver/adc.h"
#include "driver/ledc.h"

/* gpio macros */
#define BTN_PIN             /* button */
#define LDR_PIN             /* light dependent resistor */
#define POT_PIN             /* potentiometer */
#define LED_BLINK_PIN       /* led that blinks */
#define LED_PWM_PIN         /* pwm controled led */

/* adcs macros */
#define ADC_LDR
#define ADC_POT

/* configs */
ledc_timer_config_t ledc_timer = {
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_13_BIT,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 1000,
    .clk_cfg = LEDC_AUTO_CLK
};

ledc_channel_config_t ledc_channel = {
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .timer_sel = LEDC_TIMER_0,
    .intr_type = LEDC_INTR_DISABLE,
    .gpio_num = LED_BLINK_PIN,
    .duty = 0,
    .hpoint = 0
};

/* FUNCTIONS SECTION *****************************/
void gpio_confs(void)
{
    /* configuring gpios */
    gpio_set_direction(BTN_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(LED_BLINK_PIN, GPIO_MODE_OUTPUT);
    //gpio_set_direction(LED_PWM_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LDR_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(POT_PIN, GPIO_MODE_INPUT);

    /* configuring adcs */
    adc1_config_width(ADC_BITWIDTH_12);
    adc1_config_channel_atten(ADC_LDR, );

    adc2_config_width(ADC_BITWIDTH_12);
    adc2_config_channel_atten(ADC_POT, );
}

/* MAIN ******************************************/
void app_main(void)
{
    volatile mode =0;
    /* pwm led configuration */
    ledc_timer_config(&ledc_timer);
    ledc_channel_config(&ledc_channel);



    while (1)
    {
        checa botao

    }
}