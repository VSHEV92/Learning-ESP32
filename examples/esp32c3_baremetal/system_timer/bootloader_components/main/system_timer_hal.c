#include "esp_log.h"
#include "stdbool.h"
#include "sdkconfig.h"
#include "custom_init.h"
#include "hal/systimer_hal.h"
#include "hal/systimer_ll.h"

#define SYSTIMER_UNIT0 0
#define SYSTIMER_TARGET0 0

static const char *TAG = "HAL";

// In esp32c3 system timer is always clocked by 16 MHz
uint64_t ticks_to_us(uint64_t ticks) {
    return ticks / 16;
}
uint64_t us_to_ticks(uint64_t us) {
    return us * 16;
}

static systimer_hal_tick_rate_ops_t systimer_ops = {
    .ticks_to_us = ticks_to_us,
    .us_to_ticks = us_to_ticks
} ;

static systimer_hal_context_t systimer_context;

void __attribute__((noreturn)) call_start_cpu0(void)
{
    // run custom soc init
    custom_init();

    // ------------------------------------------
    //  Init system to 1 second period mode fire
    // ------------------------------------------

    // 1. Init HAl context structure
    systimer_hal_set_tick_rate_ops(&systimer_context, &systimer_ops);

    // 2. Enable counter clock
    systimer_hal_init(&systimer_context);

    // 3. Configure comparator
    systimer_hal_connect_alarm_counter(&systimer_context, SYSTIMER_TARGET0, SYSTIMER_UNIT0);
    systimer_hal_select_alarm_mode(&systimer_context, SYSTIMER_TARGET0, SYSTIMER_ALARM_MODE_PERIOD);
    systimer_hal_set_alarm_period(&systimer_context, SYSTIMER_TARGET0, CONFIG_SYSTEM_TIMER_PERIOD_MS * 1000);

    // 4. Enable comparator interrupt
    systimer_hal_enable_alarm_int(&systimer_context, SYSTIMER_TARGET0);
    
    // 5. Enable counter
    systimer_hal_enable_counter(&systimer_context, SYSTIMER_UNIT0);

    while(1){
        // poll interrupt status
        while( !systimer_ll_is_alarm_int_fired(systimer_context.dev, SYSTIMER_TARGET0) );

        // clear interrupt
        systimer_ll_clear_alarm_int(systimer_context.dev, SYSTIMER_TARGET0);

        // print system conunter value 
        uint64_t systimer_value = systimer_hal_get_counter_value(&systimer_context, SYSTIMER_UNIT0);
        ESP_EARLY_LOGI(TAG, "System timer counter value: %llu", systimer_value);
    }
}
