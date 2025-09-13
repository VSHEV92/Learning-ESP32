
#include "sdkconfig.h"
#include "max7219.h"
#include "esp_rom_sys.h"
#include "custom_init.h"

#define MAX7219_INTENSITY_CHANGE_DELAY_US 20000

// rom delay function suppose CPU clock id XTAL 20 MHz, but custom init set it to PLL 160 Mhz
#define MAX7219_INTENSITY_CHANGE_DELAY_RECALC ((MAX7219_INTENSITY_CHANGE_DELAY_US) * (160/20))

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

void __attribute__((noreturn)) call_start_cpu0(void)
{
    // disable boot watchdog timers
    custom_init();

    max7219_config_t max7219_cfg = {
        .mosi = CONFIG_MAX7219_MOSI_GPIO,
        .sclk = CONFIG_MAX7219_CLK_GPIO,
        .cs = CONFIG_MAX7219_CS_GPIO,
        .decode_mask = 0,
        .intensity = 0,
        .scan_limit = 7
    };
    max7219_init(&max7219_cfg);

    for (int i = 0; i < 8; i++) {
        max7219_send(MAX7219_DATA_REGS[i], data[i]);
    }

    while (1) {
        for (int i = 0; i < MAX7219_INTENSITY_LEVELS - 2; i++) {
            max7219_intensity_set(i);
            esp_rom_delay_us(MAX7219_INTENSITY_CHANGE_DELAY_RECALC);
        }

        for (int i = MAX7219_INTENSITY_LEVELS - 1; i > 0; i--) {
            max7219_intensity_set(i);
            esp_rom_delay_us(MAX7219_INTENSITY_CHANGE_DELAY_RECALC);
        }
    }
}
