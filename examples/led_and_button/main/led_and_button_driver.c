#include "led_and_button.h"

void configure_driver() {
    gpio_config_t gpio_cfg;

    // gpio_config - is main gpio driver function for gpio pin configuration
    // It configure following settings:
    // - pullup/pulldown resisstors using pull_up_en/pull_down_en gpio_config_t field
    // - input/output enable using mode gpio_config_t field
    // - push-pull/open-drain output using mode gpio_config_t field
    // - IO MUX as GPIO Matrix calling gpio_hal_iomux_func_sel function with PIN_FUNC_GPIO parameter 
    // - GPIO Matrix Mux source as gpio out register calling esp_rom_gpio_connect_out_signal with SIG_GPIO_OUT_IDX parameter
    // - disable output inversion calling esp_rom_gpio_connect_out_signal with false parameter
    // - disable output enable inversion calling esp_rom_gpio_connect_out_signal with false parameter
    // Driver strenght, pin filter and synchronizer should be configured separetly 

    // config button gpio pin as input with pull up resistor
    gpio_cfg = (gpio_config_t){
        .pin_bit_mask = BIT64(CONFIG_BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = true,
        .pull_down_en = false,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&gpio_cfg);

    // config button gpio pin as output
    gpio_cfg = (gpio_config_t){
        .pin_bit_mask = BIT64(CONFIG_LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = false,
        .pull_down_en = false,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&gpio_cfg);
}

void poll_driver() {
    uint32_t button_value;

    button_value = gpio_get_level(CONFIG_BUTTON_GPIO);
    gpio_set_level(CONFIG_LED_GPIO, button_value);
}
