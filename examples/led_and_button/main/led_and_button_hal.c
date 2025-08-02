#include "hal/gpio_hal.h"
#include "esp_rom_gpio.h"
#include "sdkconfig.h"

// Get GPIO registers structure
// For HAL we need wrapper of this structure with gpio_hal_context_t type
static gpio_hal_context_t gpio_hal = {
    .dev = GPIO_HAL_GET_HW(GPIO_PORT_0)
};

void configure_hal() {
    /*
     * Configure button gpio pin as input with pull up resistor
     */

    // 1. Configure pullup/pulldown resistors 
    gpio_hal_pullup_en(&gpio_hal, CONFIG_BUTTON_GPIO);
    gpio_hal_pulldown_dis(&gpio_hal, CONFIG_BUTTON_GPIO);

    // 2. Configure input/output enable
    gpio_hal_input_enable(&gpio_hal, CONFIG_BUTTON_GPIO);
    gpio_hal_output_disable(&gpio_hal, CONFIG_BUTTON_GPIO);

    /*
     * Configure led gpio pin as output
     */

    // 1. Configure GPIO Matrix Mux as gpio out regiser, disable output and output enable inversion
    esp_rom_gpio_connect_out_signal(CONFIG_LED_GPIO, SIG_GPIO_OUT_IDX, false, false);

    // 2. Configure IO Mux to get data from GPIO Matrix
    //    GPIO_PIN_MUX_REG is array that contain IO MUX register's addresses
    uint32_t io_reg = GPIO_PIN_MUX_REG[CONFIG_LED_GPIO];
    gpio_hal_iomux_func_sel(io_reg, PIN_FUNC_GPIO);

    // 3. Disable open-drain
    gpio_hal_od_disable(&gpio_hal, CONFIG_LED_GPIO);

    // 4. Disable pullup/pulldown registers
    gpio_hal_pullup_dis(&gpio_hal, CONFIG_LED_GPIO);
    gpio_hal_pulldown_dis(&gpio_hal, CONFIG_LED_GPIO);

    // 5. Configure input/output enable
    gpio_hal_input_disable(&gpio_hal, CONFIG_LED_GPIO);
    gpio_hal_output_enable(&gpio_hal, CONFIG_LED_GPIO);
}

void poll_hal() {
    uint32_t button_value;

    button_value = gpio_hal_get_level(&gpio_hal, CONFIG_BUTTON_GPIO);
    gpio_hal_set_level(&gpio_hal, CONFIG_LED_GPIO, button_value);
}
