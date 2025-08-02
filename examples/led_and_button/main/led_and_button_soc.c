#include "soc/soc.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "sdkconfig.h"

// GPIO_PIN_MUX_REG - array of GPIO IO MUX register's addresses
// REG_WRITE(register, value) - write data to given register
// REG_SET_BIT(register, bit_number) - set bit_number bit in register 
// REG_GET_BIT(register, bit_number) - get bit_number bit in register 
// REG_CLR_BIT(register, bit_number) - clear bit_number bit in register 
// REG_SET_FIELD(regiser, field_name, value) - set field in register

void configure_soc() {
    /*
     * Get GPIO register's addresses
     */
    uint32_t BUTTUN_IO_MUX_REG = IO_MUX_GPIO0_REG + 4*CONFIG_BUTTON_GPIO;

    uint32_t LED_FUNC_OUT_SEL_CFG_REG = GPIO_FUNC0_OUT_SEL_CFG_REG + 4*CONFIG_LED_GPIO;
    uint32_t LED_GPIO_PIN_REG = GPIO_PIN0_REG + 4*CONFIG_LED_GPIO;
    uint32_t LED_IO_MUX_REG = IO_MUX_GPIO0_REG + 4*CONFIG_LED_GPIO;

    /*
     * Configure button gpio pin as input with pull up resistor
     */

    // -------- IO_MUX_GPIOn_REG -------
    
    // 1. Enable pullup resistors 
    REG_SET_BIT(BUTTUN_IO_MUX_REG, FUN_PU);

    // 2. Disable pulldown resistors
    REG_CLR_BIT(BUTTUN_IO_MUX_REG, FUN_PD);

    // 3. Set input enable
    REG_SET_BIT(BUTTUN_IO_MUX_REG, FUN_IE);


    // -------- GPIO_ENABLE_REG -------
    
    // 1. Clear output enable. We set 1 to button gpio in GPIO_ENABLE_W1TC_REG
    REG_WRITE(GPIO_ENABLE_W1TC_REG, 1 << CONFIG_BUTTON_GPIO);


    /*
     * Configure led gpio pin as output
     */

    // -------- GPIO_FUNCn_OUT_SEL_CFG_REG -------

    // 1. Configure GPIO Matrix Mux as gpio out regiser  
    REG_WRITE(LED_FUNC_OUT_SEL_CFG_REG, SIG_GPIO_OUT_IDX);

    // 2. Disable output inversion 
    REG_CLR_BIT(LED_FUNC_OUT_SEL_CFG_REG, GPIO_FUNC0_OUT_INV_SEL_S);

    // 3. Disable output enable inversion 
    REG_CLR_BIT(LED_FUNC_OUT_SEL_CFG_REG, GPIO_FUNC0_OEN_INV_SEL_S);


    // -------- GPIO_PINn_REG -------

    // 1. Disable open-drain
    REG_CLR_BIT(LED_GPIO_PIN_REG, GPIO_PIN_PAD_DRIVER_LSB);


    // -------- IO_MUX_GPIOn_REG -------

    // 1. Configure IO Mux to get data from GPIO Matrix
    REG_SET_FIELD(LED_IO_MUX_REG, MCU_SEL, PIN_FUNC_GPIO);
    
    // 2. Disable pullup resistors 
    REG_CLR_BIT(LED_IO_MUX_REG, FUN_PU);

    // 3. Disable pulldown resistors
    REG_CLR_BIT(LED_IO_MUX_REG, FUN_PD);

    // 4. Clear input enable
    REG_CLR_BIT(LED_IO_MUX_REG, FUN_IE);


    // -------- GPIO_ENABLE_REG -------
    
    // 1. Set output enable. We set 1 to button gpio in GPIO_ENABLE_W1TS_REG
    REG_WRITE(GPIO_ENABLE_W1TS_REG, 1 << CONFIG_LED_GPIO);
}

void poll_soc() {
    uint32_t button_value;

    button_value = REG_GET_BIT(GPIO_IN_REG, 1 << CONFIG_BUTTON_GPIO);

    if (button_value) {
        REG_WRITE(GPIO_OUT_W1TS_REG, 1 << CONFIG_LED_GPIO);
    }
    else {
        REG_WRITE(GPIO_OUT_W1TC_REG, 1 << CONFIG_LED_GPIO);
    }
}
