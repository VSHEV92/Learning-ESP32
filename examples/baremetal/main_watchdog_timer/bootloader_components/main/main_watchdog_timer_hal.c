#include "esp_log.h"
#include "stdbool.h"
#include "custom_init.h"
#include "hal/clk_gate_ll.h"
#include "hal/wdt_hal.h"

static const char *TAG = "HAL";

static wdt_hal_context_t mwdt_context;

void __attribute__((noreturn)) call_start_cpu0(void)
{
    // run custom soc init
    custom_init();

    // ------------------------------------------
    //  Init system to 1 second period mode fire
    // ------------------------------------------

    // 1. Enable main watchdog timer clock
    periph_ll_enable_clk_clear_rst(PERIPH_TIMG1_MODULE);
    
    // 2. Init watch dog timer context
    wdt_hal_init(&mwdt_context, WDT_MWDT1, 8000, false);

    // 3. Disable write protection
    wdt_hal_write_protect_disable(&mwdt_context);

    // 4. Select clock source 
    mwdt_ll_set_clock_source(mwdt_context.mwdt_dev, MWDT_CLK_SRC_APB);

    // 5. Configure timeout and action for each stage. Stage 1 = reset after 1 sec, other not used  
    wdt_hal_config_stage(&mwdt_context, WDT_STAGE0, 10000, WDT_STAGE_ACTION_RESET_SYSTEM);
    wdt_hal_config_stage(&mwdt_context, WDT_STAGE1, 0, WDT_STAGE_ACTION_OFF);
    wdt_hal_config_stage(&mwdt_context, WDT_STAGE2, 0, WDT_STAGE_ACTION_OFF);
    wdt_hal_config_stage(&mwdt_context, WDT_STAGE3, 0, WDT_STAGE_ACTION_OFF);

    // 6. Enable watchdog
    wdt_hal_enable(&mwdt_context);

    // 7. Enable write protection
    wdt_hal_write_protect_enable(&mwdt_context);

    ESP_EARLY_LOGI(TAG, "Wait for watchdog reset");
    while(1);
    
}
