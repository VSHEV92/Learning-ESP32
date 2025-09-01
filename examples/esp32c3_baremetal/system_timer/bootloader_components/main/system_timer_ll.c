#include "esp_log.h"
#include "stdbool.h"
#include "sdkconfig.h"
#include "custom_init.h"
#include "hal/systimer_ll.h"

#define SYSTIMER_UNIT0 0
#define SYSTIMER_TARGET0 0

static const char *TAG = "LL";

static systimer_dev_t* systimer_dev = &SYSTIMER;


void __attribute__((noreturn)) call_start_cpu0(void)
{
    // in esp32c3 system timer period is always 16 MHz
    uint64_t period_ticks = 16000000 / 1000 * CONFIG_SYSTEM_TIMER_PERIOD_MS; 
    uint64_t systimer_value;

    // run custom soc init
    custom_init();

    // ------------------------------------------
    //  Init system to 1 second period mode fire
    // ------------------------------------------

    // 1. Enable system timer clock
    systimer_ll_enable_clock(systimer_dev, true);
    
    // 2. Disable counter
    systimer_ll_enable_counter(systimer_dev, SYSTIMER_UNIT0, false);

    // 3. Set counter initial value
    systimer_ll_set_counter_value(systimer_dev, SYSTIMER_UNIT0, 0);
    systimer_ll_apply_counter_value(systimer_dev, SYSTIMER_UNIT0);

    // 4. Configure comparator
    systimer_ll_connect_alarm_counter(systimer_dev, SYSTIMER_TARGET0, SYSTIMER_UNIT0);
    systimer_ll_enable_alarm_period(systimer_dev, SYSTIMER_TARGET0);
    systimer_ll_set_alarm_period(systimer_dev, SYSTIMER_TARGET0, period_ticks);
    systimer_ll_apply_alarm_value(systimer_dev, SYSTIMER_TARGET0);

    // 5. Enable comparator
    systimer_ll_enable_alarm(systimer_dev, SYSTIMER_TARGET0, true);

    // 6. Enable comparator interrupt
    systimer_ll_enable_alarm_int(systimer_dev, SYSTIMER_TARGET0, true);
    
    // 7. Enable counter
    systimer_ll_enable_counter(systimer_dev, SYSTIMER_UNIT0, true);


    while(1){
        // poll interrupt status
        while( !systimer_ll_is_alarm_int_fired(systimer_dev, SYSTIMER_TARGET0) );

        // clear interrupt
        systimer_ll_clear_alarm_int(systimer_dev, SYSTIMER_TARGET0);

        // print system conunter value 
        // 1. Trigger value update
        systimer_ll_counter_snapshot(systimer_dev, SYSTIMER_UNIT0);
        // 2. Poll value valid flag
        while( !systimer_ll_is_counter_value_valid(systimer_dev, SYSTIMER_UNIT0) );
        // 3. Read systimer value
        systimer_value = systimer_ll_get_counter_value_high(systimer_dev, SYSTIMER_UNIT0);
        systimer_value <<= 32;
        systimer_value |= systimer_ll_get_counter_value_low(systimer_dev, SYSTIMER_UNIT0);
        ESP_EARLY_LOGI(TAG, "System timer counter value: %llu", systimer_value);
    }
}
