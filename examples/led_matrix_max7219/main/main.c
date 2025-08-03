#include <string.h>

#include "esp_log.h"
#include "sdkconfig.h"

#include <driver/spi_master.h>
#include <driver/gpio.h> // add by nopnop2002

#define HOST    SPI2_HOST

void app_main(void)
{

    spi_device_handle_t spi_dev;

    // Configure SPI bus
    spi_bus_config_t cfg = {
       .mosi_io_num = 10,
       .miso_io_num = -1,
       .sclk_io_num = 8,
       .quadwp_io_num = -1,
       .quadhd_io_num = -1,
       .max_transfer_sz = 0,
       .flags = 0
    };
    ESP_ERROR_CHECK(spi_bus_initialize(HOST, &cfg, SPI_DMA_DISABLED));


    // Configure SPI device
    spi_device_interface_config_t spi_cfg;
    memset(&spi_cfg, 0, sizeof(spi_cfg));

    spi_cfg.spics_io_num = 21;
    spi_cfg.clock_speed_hz = (10000000); // 10 MHz;
    spi_cfg.mode = 0;
    spi_cfg.queue_size = 1;
    spi_cfg.flags = SPI_DEVICE_NO_DUMMY;

    ESP_ERROR_CHECK(spi_bus_add_device(HOST, &spi_cfg, &spi_dev));

    // transmit test mode
    spi_transaction_t spi_trans;
    uint16_t val = (15 << 8) | 1;

    val = (val >> 8) | (val << 8);

    memset(&spi_trans, 0, sizeof(spi_trans));
    spi_trans.length = 16;
    spi_trans.flags = SPI_TRANS_USE_TXDATA;
    spi_trans.tx_data[0] = 15;
    spi_trans.tx_data[1] = 1;

    ESP_ERROR_CHECK(spi_device_polling_transmit(spi_dev, &spi_trans));

}
