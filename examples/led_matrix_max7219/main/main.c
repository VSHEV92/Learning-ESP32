#include "sdkconfig.h"
#include <led_matrix_max7219.h>

void app_main(void)
{
    uint8_t data[] = {
        0b00000000,
        0b01101100,
        0b11111110,
        0b11111110,
        0b01111100,
        0b00111000,
        0b00010000,
        0b00000000
    };

    spi_device_handle_t spi_dev;

    max7219_config_t max7219_cfg = {
        .mosi = 10,
        .sclk = 8,
        .cs = 21,
        .decode_mask = 0,
        .intensity = 15,
        .scan_limit = 7
    };
    spi_dev = max7219_init(&max7219_cfg);

    for (int i = 0; i < 8; i++) {
        max7219_send(spi_dev, MAX7219_DATA_REGS[i], data[i]);
    }
}
