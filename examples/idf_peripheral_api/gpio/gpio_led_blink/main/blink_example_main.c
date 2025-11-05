#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BLINK_GPIO 8
#define BLINK_PERIOD_MS 1000

void app_main(void)
{
    uint8_t led_state = 0;
    
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    while (true) {
        gpio_set_level(BLINK_GPIO, led_state);
        vTaskDelay( pdMS_TO_TICKS(BLINK_PERIOD_MS) );
        led_state = !led_state;
    }
}
