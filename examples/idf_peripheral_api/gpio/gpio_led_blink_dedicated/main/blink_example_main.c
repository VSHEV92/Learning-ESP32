#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/dedic_gpio.h"

#define BLINK_GPIO 8
#define BLINK_PERIOD_MS 1000

void app_main(void)
{
    // aaray of gpio pins, that will be included to bundle
    const int gpio_array[] = {BLINK_GPIO};    

    // bundle configuration - one output pin
    dedic_gpio_bundle_config_t led_bundle_cfg = {
        .gpio_array = gpio_array,
        .array_size = 1,
        .flags = {
            .in_en = 0,
            .in_invert = 0,
            .out_en = 1,
            .out_invert = 0
        }
    };

    dedic_gpio_bundle_handle_t led_bundle;
    uint32_t led_bundle_mask;
    uint8_t led_state = 0;
    
    // create gpio bundle (configure IOMUX, GPIO Matrix to CPU GPIO CSRs, enable outputs)
    dedic_gpio_new_bundle(&led_bundle_cfg, &led_bundle);

    // get led bundle mask (CPU GPIO CSR bits that was assigned to bundle)
    dedic_gpio_get_out_mask(led_bundle, &led_bundle_mask);
    
    while (true) {
        dedic_gpio_bundle_write(led_bundle, led_bundle_mask, led_state);
        vTaskDelay( pdMS_TO_TICKS(BLINK_PERIOD_MS) );
        led_state = !led_state;
    }
}
