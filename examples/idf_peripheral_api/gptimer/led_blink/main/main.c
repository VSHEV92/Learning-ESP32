#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"

#define BLINK_GPIO 8
#define BLINK_PERIOD_MS 200
#define GPTIMER_RESOLUTION_HZ 1000000

uint8_t led_state = 0;

// deferred interrupt handler, that change led state
void toggle_led( void * pvParameter1, uint32_t ulParameter2 ) {
    gpio_set_level(BLINK_GPIO, led_state);
    led_state = !led_state;
}

// gptimer ISR handler
bool gptimer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data) {
    BaseType_t high_task_awoken = pdFALSE;
    xTimerPendFunctionCallFromISR(toggle_led, NULL, 0, &high_task_awoken);
    return high_task_awoken;
}

void app_main(void) {

    // configure GPIO port
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    // configure timer
    gptimer_config_t timer_cfg = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = GPTIMER_RESOLUTION_HZ,
    };

    // create timer handle
    gptimer_handle_t gptimer = NULL;
    gptimer_new_timer(&timer_cfg, &gptimer);

    // configure alarm action
    gptimer_alarm_config_t alarm_cfg = {
        .alarm_count = (GPTIMER_RESOLUTION_HZ * BLINK_PERIOD_MS / 1000 ), // period = 1s
        .reload_count = 0,                                                // reset timer to zero on alarm
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_set_alarm_action(gptimer, &alarm_cfg);

    // set alarm callback
    gptimer_event_callbacks_t gptimer_alarm_cbs = {
        .on_alarm = gptimer_alarm_cb,
    };
    gptimer_register_event_callbacks(gptimer, &gptimer_alarm_cbs, NULL);

    // enable and start timer 
    gptimer_enable(gptimer);
    gptimer_start(gptimer);

    while(true){}
}
