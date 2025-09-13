#include "max7219.h"
#include "esp_rom_sys.h"

#define __DECLARE_RCC_ATOMIC_ENV 1
#define CUSTOM_INIT_APB_CLK_FREQ 160000000
#define ESPC3_GPIO &GPIO
#define ESPC3_SPI &GPSPI2
#define ESPC3_SPI_CS_NUMBER 0
#define ESPC3_SCLK_DUTY_CYCLE 0

uint8_t MAX7219_DATA_REGS [] = {
    MAX7219_DATA0_REG,
    MAX7219_DATA1_REG,
    MAX7219_DATA2_REG,
    MAX7219_DATA3_REG,
    MAX7219_DATA4_REG,
    MAX7219_DATA5_REG,
    MAX7219_DATA6_REG,
    MAX7219_DATA7_REG
};

static void max7219_init_spi(max7219_config_t* max7219_cfg) {

    // --------------------------------------------------------
    //                   Configure SPI Pins
    // --------------------------------------------------------

    // 1. Configure GPIO Matrix Mux to connect spi to gpio   
    esp_rom_gpio_connect_out_signal(max7219_cfg->sclk, spi_periph_signal[MAX7219_HOST].spiclk_out, false, false);
    esp_rom_gpio_connect_out_signal(max7219_cfg->mosi, spi_periph_signal[MAX7219_HOST].spid_out, false, false);
    esp_rom_gpio_connect_out_signal(max7219_cfg->cs, spi_periph_signal[MAX7219_HOST].spics_out[ESPC3_SPI_CS_NUMBER], false, false);

    // 2. Configure IO Mux to get data from GPIO Matrix
    gpio_ll_iomux_func_sel(GPIO_PIN_MUX_REG[max7219_cfg->sclk], PIN_FUNC_GPIO);
    gpio_ll_iomux_func_sel(GPIO_PIN_MUX_REG[max7219_cfg->mosi], PIN_FUNC_GPIO);
    gpio_ll_iomux_func_sel(GPIO_PIN_MUX_REG[max7219_cfg->cs], PIN_FUNC_GPIO);

    // 3. Disable open-drain
    gpio_ll_od_disable(ESPC3_GPIO, max7219_cfg->sclk);
    gpio_ll_od_disable(ESPC3_GPIO, max7219_cfg->mosi);
    gpio_ll_od_disable(ESPC3_GPIO, max7219_cfg->cs);

    // 4. Disable pullup registers
    gpio_ll_pullup_dis(ESPC3_GPIO, max7219_cfg->sclk);
    gpio_ll_pullup_dis(ESPC3_GPIO, max7219_cfg->mosi);
    gpio_ll_pullup_dis(ESPC3_GPIO, max7219_cfg->cs);

    // 5. Disable pulldown registers
    gpio_ll_pulldown_dis(ESPC3_GPIO, max7219_cfg->sclk);
    gpio_ll_pulldown_dis(ESPC3_GPIO, max7219_cfg->mosi);
    gpio_ll_pulldown_dis(ESPC3_GPIO, max7219_cfg->cs);

    // 6. Configure input enable
    gpio_ll_input_disable(ESPC3_GPIO, max7219_cfg->sclk);
    gpio_ll_input_disable(ESPC3_GPIO, max7219_cfg->mosi);
    gpio_ll_input_disable(ESPC3_GPIO, max7219_cfg->cs);
    
    // 7. Configure output enable
    gpio_ll_output_enable(ESPC3_GPIO, max7219_cfg->sclk);
    gpio_ll_output_enable(ESPC3_GPIO, max7219_cfg->mosi);
    gpio_ll_output_enable(ESPC3_GPIO, max7219_cfg->cs);

    // --------------------------------------------------------
    //          Clock Enable and Reset SPI Periferal
    // --------------------------------------------------------

    // 1. Enable SPI periferal APB clock
    spi_ll_enable_bus_clock(MAX7219_HOST, true);
    // 2. Reset SPI periferal
    spi_ll_reset_register(MAX7219_HOST);
    
    // --------------------------------------------------------
    //                 Configure SPI Periferal
    // --------------------------------------------------------

    // 1. SPI initial configureation
    //    - zeroing CS setup and hold time
    //    - disable input/output highpart, all data registers are used
    //    - zeroing SLAVE register
    //    - zeroing USER register
    //    - enable master clock and set source to PLL
    //    - disable dma
    spi_ll_master_init(ESPC3_SPI);

    // 2. Set SCLK clock frequency and enable SCLK output clock 
    spi_ll_master_set_clock(ESPC3_SPI, CUSTOM_INIT_APB_CLK_FREQ, MAX7219_CLK_FREQ, ESPC3_SCLK_DUTY_CYCLE);
    spi_ll_enable_clock(MAX7219_HOST, true);

    // 3. Set transmittion with MSB first
    spi_ll_set_tx_lsbfirst(ESPC3_SPI, false);

    // 4. Disable hald-duplex and SIO mode
    spi_ll_set_half_duplex(ESPC3_SPI, false);
    spi_ll_set_sio_mode(ESPC3_SPI, false);

    // 5. Enable selected CS
    spi_ll_master_select_cs(ESPC3_SPI, ESPC3_SPI_CS_NUMBER);

    // 6. Configure clock mode. CPOL = 0, CPHA = 0
    spi_ll_master_set_mode(ESPC3_SPI, 0);

    // 7. Configure master state machine
    //  - disable command state
    //  - enable address state and set it's bit length
    //  - disable dummy state
    //  - enable mosi state and set it's bit length
    //  - disable miso state
    spi_ll_set_command_bitlen(ESPC3_SPI, 0);
    spi_ll_set_addr_bitlen(ESPC3_SPI, MAX7219_ADDR_SIZE);
    spi_ll_set_dummy(ESPC3_SPI, 0);
    spi_ll_set_mosi_bitlen(ESPC3_SPI, MAX7219_DATA_SIZE);
    spi_ll_enable_mosi(ESPC3_SPI, true);
    spi_ll_enable_miso(ESPC3_SPI, false);

    // 8. Apply configuration setting
    spi_ll_apply_config(&GPSPI2);
}

void max7219_init(max7219_config_t* max7219_cfg) {

    max7219_init_spi(max7219_cfg);

    max7219_display_test_disable();
    max7219_shutdown_enable();
    max7219_scan_limit_set(max7219_cfg->scan_limit);
    max7219_decode_mask_set(max7219_cfg->decode_mask);
    max7219_intensity_set(max7219_cfg->intensity);
    max7219_shutdown_disable();
}

                                                    
void max7219_send(uint8_t addr, uint8_t data) {
    // 1. Load data to spi internal registers
    uint8_t buff[4] = {data, 0, 0, 0};
    spi_ll_write_buffer(&GPSPI2, buff, MAX7219_DATA_SIZE);
    
    // 2. Set transaction address
    spi_ll_set_address(&GPSPI2, addr, MAX7219_ADDR_SIZE, 0);

    // 3. Apply address and data settings
    spi_ll_apply_config(&GPSPI2);

    // 4. Start transaction
    spi_ll_user_start(&GPSPI2);

    // 5. Poll raw transaction done status
    while (!spi_ll_usr_is_done(&GPSPI2)){}; 

    // 6. Clear raw transaction done status
    spi_ll_clear_int_stat(&GPSPI2);
}


void max7219_shutdown_enable() {
    max7219_send(MAX7219_SHUTDOWN_REG, 0);
}


void max7219_shutdown_disable() {
    max7219_send(MAX7219_SHUTDOWN_REG, 1);
}


void max7219_display_test_enable() {
    max7219_send(MAX7219_DISPLAY_TEST_REG, 1);
}


void max7219_display_test_disable() {
    max7219_send(MAX7219_DISPLAY_TEST_REG, 0);
}


void max7219_scan_limit_set(uint8_t limit) {
    max7219_send(MAX7219_SCAN_LIMIT_REG, limit);
}


void max7219_decode_mask_set(uint8_t mask) {
    max7219_send(MAX7219_DECODE_MODE_REG, mask);
}


void max7219_intensity_set(uint8_t intensity) {
    max7219_send(MAX7219_INTENSITY_REG, intensity);
}
