
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/sdm.h"

#define SDM_GPIO_LED_NUMBER   8
#define SDM_FADE_PERIOD_MS    3000
#define SDM_FREQUENCY_HZ      1*1000*1000

void app_main(void)
{

    // configure sdm channel
    sdm_config_t sdm_cfg = {
        .clk_src = SDM_CLK_SRC_APB,          // set clock source as APB clock
        .gpio_num = SDM_GPIO_LED_NUMBER,     // gpio led pin number
        .sample_rate_hz = SDM_FREQUENCY_HZ,  // sdm sample rate
        .flags = {
            .invert_out = true,
            .io_loop_back = false
        }
    };

    // initialize and enable sdm channel
    sdm_channel_handle_t sdm_chan;
    sdm_new_channel(&sdm_cfg, &sdm_chan);
    sdm_channel_enable(sdm_chan);


    while (1) {
        for (int duty = -128; duty <= 127; duty++) {
            sdm_channel_set_pulse_density(sdm_chan, (int8_t)duty);
            vTaskDelay( pdMS_TO_TICKS( SDM_FADE_PERIOD_MS / 256) );
        }
    }
}
