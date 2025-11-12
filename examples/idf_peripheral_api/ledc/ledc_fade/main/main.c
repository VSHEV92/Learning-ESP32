#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define LEDC_PWD_FREQUENCY 1000
#define LEDC_GPIO_PIN 8
#define LEDC_FADE_STEPS 20
#define LEDC_FADE_PERIOD_MS 2000
#define LEDC_DUTY_MAX_VALUE (1 << (LEDC_TIMER_10_BIT - 1))

void app_main(void)
{
    // configure ledc timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,     // timer mode
        .duty_resolution = LEDC_TIMER_10_BIT,  // resolution of PWM duty
        .freq_hz = LEDC_PWD_FREQUENCY,         // frequency of PWM signal
        .timer_num = LEDC_TIMER_0,             // timer index
        .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
    };
    ledc_timer_config(&ledc_timer);

    // configure ledc channel
    ledc_channel_config_t ledc_channel = {
        .channel    = LEDC_CHANNEL_0,          // channel number
        .timer_sel  = LEDC_TIMER_0,            // timer for selected channel
        .duty       = 0,                       // duty cycle (0 < 2**duty_resolution), init duty cycle is zero
        .gpio_num   = LEDC_GPIO_PIN,           // ledc gpio number
        .speed_mode = LEDC_LOW_SPEED_MODE,     // timer mode
        .hpoint     = 0,                       // low to high PWD threshhold, use for PWD phase shift
        .flags.output_invert = 1,              // invert output
    };
    ledc_channel_config(&ledc_channel);


    while (1) {
        uint32_t duty_cycle = 0;
        for (int i = 0; i < LEDC_FADE_STEPS; i++) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty_cycle);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            duty_cycle += (LEDC_DUTY_MAX_VALUE / LEDC_FADE_STEPS);
            vTaskDelay( pdMS_TO_TICKS( LEDC_FADE_PERIOD_MS / LEDC_FADE_STEPS) );
        }
    }
}
