#include "esp_log.h"
#include "stdbool.h"
#include "sdkconfig.h"
#include "custom_init.h"
#include "hal/timer_ll.h"

#define TIMER_GROUP 1
#define TIMER_NUMBER_IN_GROUP 0
#define __DECLARE_RCC_RC_ATOMIC_ENV 1

static const char *TAG = "LL";

static timg_dev_t* timer_dev = TIMER_LL_GET_HW(TIMER_GROUP);


void __attribute__((noreturn)) call_start_cpu0(void)
{
    // Configure in esp32c3 timer period to 1 MHz
    uint64_t period_ticks = 1000000 / 1000 * CONFIG_GENERAL_PERPOSE_TIMER_PERIOD_MS; 

    // run custom soc init
    custom_init();

    // ------------------------------------------
    //  Init general perpose to 1 second periodic fire
    // ------------------------------------------

    // 1. Enable timer clock
    timer_ll_enable_bus_clock(TIMER_GROUP, true);
    
    // 2. Disable counter
    timer_ll_enable_counter(timer_dev, TIMER_NUMBER_IN_GROUP, false);

    // 3. Select clock source to APB (80 MHz) and configure clock divider to 80
    //    So that timer clock will be 1 Mhz
    timer_ll_set_clock_source(timer_dev, TIMER_NUMBER_IN_GROUP, GPTIMER_CLK_SRC_APB);
    timer_ll_set_clock_prescale(timer_dev, TIMER_NUMBER_IN_GROUP, 80);
    timer_ll_enable_clock(timer_dev, TIMER_NUMBER_IN_GROUP, true);

    // 4. Select timer mode as incremente
    timer_ll_set_count_direction(timer_dev, TIMER_NUMBER_IN_GROUP, GPTIMER_COUNT_UP);
    
    // 5. Configure and enable comparator
    timer_ll_set_alarm_value(timer_dev, TIMER_NUMBER_IN_GROUP, period_ticks); 
    timer_ll_enable_alarm(timer_dev, TIMER_NUMBER_IN_GROUP, true);

    // 6. Enable auto-reload and load zero value to counter
    timer_ll_enable_auto_reload(timer_dev, TIMER_NUMBER_IN_GROUP, true);
    timer_ll_set_reload_value(timer_dev, TIMER_NUMBER_IN_GROUP, 0);
    timer_ll_trigger_soft_reload(timer_dev, TIMER_NUMBER_IN_GROUP);
    
    // 7. Enable comparator interrupt (mask - [WDT_EN, TO_EN])
    timer_ll_enable_intr(timer_dev, 0b01, true);
    
    // 8. Enable counter
    timer_ll_enable_counter(timer_dev, TIMER_NUMBER_IN_GROUP, true);


    while(1){
        // poll interrupt status
        while( !timer_ll_get_intr_status(timer_dev) );

        // clear interrupt
        timer_ll_clear_intr_status(timer_dev, 0b01);

        // reenable alarm comparator
        timer_ll_enable_alarm(timer_dev, TIMER_NUMBER_IN_GROUP, true);

        // print message about timer reload
        ESP_EARLY_LOGI(TAG, "Timer reloaded");
    }
}
