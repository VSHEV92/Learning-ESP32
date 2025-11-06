#include "driver/gpio.h"

#define LED_GPIO 8
#define BUTTON_GPIO 21

#define ESP_INTR_FLAG_DEFAULT 0

uint8_t led_state = 0;

void gpio_button_handler(void* arg) {
    gpio_set_level(LED_GPIO, led_state);
    led_state = !led_state;
}

void app_main(void) {
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
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // configure interrupt
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(BUTTON_GPIO, gpio_button_handler, (void*)NULL);

    while (true) {}
}
