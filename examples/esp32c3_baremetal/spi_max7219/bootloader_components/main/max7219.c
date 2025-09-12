#include "max7219.h"

#define __DECLARE_RCC_ATOMIC_ENV 1
#define APB_CLK_FREQ 160000000

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
    gpio_ll_output_enable(&GPIO, max7219_cfg->mosi);
    esp_rom_gpio_connect_out_signal(max7219_cfg->mosi, spi_periph_signal[MAX7219_HOST].spid_out, false, false);

    gpio_ll_output_enable(&GPIO, max7219_cfg->sclk);
    esp_rom_gpio_connect_out_signal(max7219_cfg->sclk, spi_periph_signal[MAX7219_HOST].spiclk_out, false, false);

    gpio_ll_output_enable(&GPIO, max7219_cfg->cs);
    esp_rom_gpio_connect_out_signal(max7219_cfg->cs, spi_periph_signal[MAX7219_HOST].spics_out[0], false, false);
    gpio_ll_iomux_func_sel(GPIO_PIN_MUX_REG[max7219_cfg->cs], PIN_FUNC_GPIO);

    spi_ll_enable_bus_clock(MAX7219_HOST, true);
    spi_ll_reset_register(MAX7219_HOST);
    
    spi_ll_master_init(&GPSPI2);
    spi_ll_enable_int(&GPSPI2);
    spi_ll_set_int_stat(&GPSPI2);
    spi_ll_apply_config(&GPSPI2);
  
    spi_ll_enable_clock(MAX7219_HOST, 1);

    spi_ll_master_set_clock(&GPSPI2, APB_CLK_FREQ, MAX7219_CLK_FREQ, 0);
    spi_ll_set_tx_lsbfirst(&GPSPI2, 0);
    spi_ll_master_set_mode(&GPSPI2, 0);
    spi_ll_set_half_duplex(&GPSPI2, 0);
    spi_ll_set_sio_mode(&GPSPI2, 0);
    spi_ll_master_select_cs(&GPSPI2, 0);

    spi_ll_set_mosi_bitlen(&GPSPI2, MAX7219_DATA_SIZE);

    spi_ll_set_addr_bitlen(&GPSPI2, MAX7219_ADDR_SIZE);
    spi_ll_enable_mosi(&GPSPI2, 1);
}

void max7219_init(max7219_config_t* max7219_cfg) {

    max7219_init_spi(max7219_cfg);

    max7219_shutdown_enable();
    max7219_scan_limit_set(max7219_cfg->scan_limit);
    max7219_decode_mask_set(max7219_cfg->decode_mask);
    max7219_intensity_set(max7219_cfg->intensity);
    max7219_shutdown_disable();
}

                                                    
void max7219_send(uint8_t addr, uint8_t data) {
    uint8_t buff[4] = {data, 0, 0, 0};
    spi_ll_set_address(&GPSPI2, addr, MAX7219_ADDR_SIZE, 0);
    spi_ll_write_buffer(&GPSPI2, buff, MAX7219_DATA_SIZE);

    spi_ll_apply_config(&GPSPI2);
    spi_ll_user_start(&GPSPI2);

    while (!spi_ll_usr_is_done(&GPSPI2)){}; 
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
