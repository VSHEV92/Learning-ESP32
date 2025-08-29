#include "esp_log.h"
#include "stdbool.h"
#include "custom_init.h"
#include "hal/clk_gate_ll.h"
#include "hal/mwdt_ll.h"

static const char *TAG = "LL";

static timg_dev_t* mwdt_dev = &TIMERG1;

void __attribute__((noreturn)) call_start_cpu0(void)
{
    // run custom soc init
    custom_init();

    // ------------------------------------------
    //  Init system to 1 second period mode fire
    // ------------------------------------------

    // 1. Enable main watchdog timer clock
    periph_ll_enable_clk_clear_rst(PERIPH_TIMG1_MODULE);
    
    // 2. Disable write protection
    mwdt_ll_write_protect_disable(mwdt_dev);

    // 3. Disable watchdog
    mwdt_ll_disable(mwdt_dev);

    // 4. Set and enable clock to 10kHz. Select clock source as APB (80MHz) and set prescaler to 8000
    mwdt_ll_set_clock_source(mwdt_dev, MWDT_CLK_SRC_APB);
    mwdt_ll_set_prescaler(mwdt_dev, 8000);
    mwdt_ll_enable_clock(mwdt_dev, true);

    // 5. Configure timeout and action for each stage. Stage 1 = reset after 1 sec, other not used  
    mwdt_ll_config_stage(mwdt_dev, WDT_STAGE0, 10000, WDT_STAGE_ACTION_RESET_SYSTEM);
    mwdt_ll_config_stage(mwdt_dev, WDT_STAGE1, 0, WDT_STAGE_ACTION_OFF);
    mwdt_ll_config_stage(mwdt_dev, WDT_STAGE2, 0, WDT_STAGE_ACTION_OFF);
    mwdt_ll_config_stage(mwdt_dev, WDT_STAGE3, 0, WDT_STAGE_ACTION_OFF);

    // 6. Enable watchdog
    mwdt_ll_enable(mwdt_dev);

    // 7. Enable write protection
    mwdt_ll_write_protect_enable(mwdt_dev);

    ESP_EARLY_LOGI(TAG, "Wait for watchdog reset");
    while(1);
    
}
