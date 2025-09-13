#include "driver/max7219.h"

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


spi_device_handle_t max7219_init(max7219_config_t* max7219_cfg) {

    // Configure SPI bus
    spi_bus_config_t bus_cfg = {
       .mosi_io_num = max7219_cfg->mosi,
       .miso_io_num = -1,
       .sclk_io_num = max7219_cfg->sclk,
       .quadwp_io_num = -1,
       .quadhd_io_num = -1,
       .flags = 0
    };
    spi_bus_initialize(MAX7219_HOST, &bus_cfg, SPI_DMA_DISABLED);

    // Configure SPI device
    spi_device_interface_config_t spi_dev_cfg;
    memset(&spi_dev_cfg, 0, sizeof(spi_dev_cfg));

    spi_dev_cfg.spics_io_num = max7219_cfg->cs;
    spi_dev_cfg.clock_speed_hz = MAX7219_CLK_FREQ;
    spi_dev_cfg.address_bits = MAX7219_ADDR_SIZE;
    spi_dev_cfg.mode = 0;
    spi_dev_cfg.queue_size = 1;
    spi_dev_cfg.flags = SPI_DEVICE_NO_DUMMY;

    spi_device_handle_t spi_dev;
    spi_bus_add_device(MAX7219_HOST, &spi_dev_cfg, &spi_dev);

    max7219_display_test_disable(spi_dev);
    max7219_shutdown_enable(spi_dev);
    max7219_scan_limit_set(spi_dev, max7219_cfg->scan_limit);
    max7219_decode_mask_set(spi_dev, max7219_cfg->decode_mask);
    max7219_intensity_set(spi_dev, max7219_cfg->intensity);
    max7219_shutdown_disable(spi_dev);

    return spi_dev;
}


void max7219_send(spi_device_handle_t spi_dev, uint8_t addr, uint8_t data) {
    spi_transaction_t spi_trans;

    memset(&spi_trans, 0, sizeof(spi_trans));
    spi_trans.length = MAX7219_DATA_SIZE;
    spi_trans.flags = SPI_TRANS_USE_TXDATA;
    spi_trans.addr = addr;
    spi_trans.tx_data[0] = data;

    spi_device_polling_transmit(spi_dev, &spi_trans);
}


void max7219_shutdown_enable(spi_device_handle_t spi_dev) {
    max7219_send(spi_dev, MAX7219_SHUTDOWN_REG, 0);
}


void max7219_shutdown_disable(spi_device_handle_t spi_dev) {
    max7219_send(spi_dev, MAX7219_SHUTDOWN_REG, 1);
}


void max7219_display_test_enable(spi_device_handle_t spi_dev) {
    max7219_send(spi_dev, MAX7219_DISPLAY_TEST_REG, 1);
}


void max7219_display_test_disable(spi_device_handle_t spi_dev) {
    max7219_send(spi_dev, MAX7219_DISPLAY_TEST_REG, 0);
}


void max7219_scan_limit_set(spi_device_handle_t spi_dev, uint8_t limit) {
    max7219_send(spi_dev, MAX7219_SCAN_LIMIT_REG, limit);
}


void max7219_decode_mask_set(spi_device_handle_t spi_dev, uint8_t mask) {
    max7219_send(spi_dev, MAX7219_DECODE_MODE_REG, mask);
}


void max7219_intensity_set(spi_device_handle_t spi_dev, uint8_t intensity) {
    max7219_send(spi_dev, MAX7219_INTENSITY_REG, intensity);
}
