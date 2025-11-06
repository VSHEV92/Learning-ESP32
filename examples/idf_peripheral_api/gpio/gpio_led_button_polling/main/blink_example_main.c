#include "driver/gpio.h"

#define LED_GPIO 8
#define BUTTON_GPIO 21

void app_main(void)
{
    uint8_t button_state = 0;
    gpio_config_t io_conf = {};

    // configure led gpio
    io_conf.pin_bit_mask = 1 << LED_GPIO;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    
    // configure button gpio
    io_conf.pin_bit_mask = 1 << BUTTON_GPIO;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    while (true) {
        button_state = gpio_get_level(BUTTON_GPIO);
        gpio_set_level(LED_GPIO, button_state);
   }
}
