#include "hal/gpio_hal.h"
#include "esp_rom_sys.h"
#include "esp_rom_gpio.h"
#include "sdkconfig.h"
#include "custom_init.h"

static gpio_hal_context_t gpio_hal = {
    .dev = GPIO_HAL_GET_HW(GPIO_PORT_0)
};

static void configure_led_gpio(gpio_hal_context_t* gpio_hal) {
    // 1. Configure GPIO Matrix Mux as gpio out regiser, disable output and output enable inversion
    esp_rom_gpio_connect_out_signal(CONFIG_LED_GPIO, SIG_GPIO_OUT_IDX, false, false);

    // 2. Configure IO Mux to get data from GPIO Matrix
    //    GPIO_PIN_MUX_REG is array that contain IO MUX register's addresses
    uint32_t io_reg = GPIO_PIN_MUX_REG[CONFIG_LED_GPIO];
    gpio_hal_iomux_func_sel(io_reg, PIN_FUNC_GPIO);

    // 3. Disable open-drain
    gpio_hal_od_disable(gpio_hal, CONFIG_LED_GPIO);

    // 4. Disable pullup/pulldown registers
    gpio_hal_pullup_dis(gpio_hal, CONFIG_LED_GPIO);
    gpio_hal_pulldown_dis(gpio_hal, CONFIG_LED_GPIO);

    // 5. Configure input/output enable
    gpio_hal_input_disable(gpio_hal, CONFIG_LED_GPIO);
    gpio_hal_output_enable(gpio_hal, CONFIG_LED_GPIO);
}

void __attribute__((noreturn)) call_start_cpu0(void)
{
    // disable boot watchdog timers
    custom_init();

    // configure led gpio as output pin
    configure_led_gpio(&gpio_hal);

    // blink loop
    int value = 0;
    while(1){
        gpio_hal_set_level(&gpio_hal, CONFIG_LED_GPIO, value);
        value = !value;
        esp_rom_delay_us(CONFIG_BLINK_PERIOD_US/2);
    }
}
