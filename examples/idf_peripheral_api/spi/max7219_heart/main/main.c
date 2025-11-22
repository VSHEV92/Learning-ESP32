#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <max7219.h>

#define MAX7219_SCLK 4
#define MAX7219_MOSI 2
#define MAX7219_CS 3

uint8_t data_8x8[] = {
    0b00000000,
    0b01101100,
    0b11111110,
    0b11111110,
    0b01111100,
    0b00111000,
    0b00010000,
    0b00000000
};

void app_main(void) {
    // configure spi bus
    spi_bus_config_t spi_bus_cfg = {
        .sclk_io_num = MAX7219_SCLK,
        .mosi_io_num = MAX7219_MOSI,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 2,
        .flags = 0
    };
    spi_bus_initialize(SPI2_HOST, &spi_bus_cfg, SPI_DMA_CH_AUTO);
    

    // configure max7219 device
    max7219_t max7219_device =
    {
        .cascade_size = 1, // number of max7219 devices connected in cacsade
        .digits = 0,       // number of digits in 7seg or lines in 8x8, 0 - maximum 8 digits 
        .mirrored = false, // mirror digits in 7seg or flip lines in 8x8 
        .bcd = false       // enable bcd 
    };
    
    max7219_init_desc(
        &max7219_device,            // pointer to max7219_t structure
        SPI2_HOST,                  // spi host, SPI2_HOST is only valid host for esp32c3 
        MAX7219_MAX_CLOCK_SPEED_HZ, // spi clock rate in Hz, set to max clock rate of max7219 - 10 MHz 
        MAX7219_CS                  // spi CS pin number 
    );
    

    // inititalize max7219 device
    // 1. Disable test
    // 2. Set max scan limit
    // 3. Set normal decode mode 
    // 4. Clear screen
    // 5. Set minimal brightness
    max7219_init(&max7219_device);


    // display heart on 8x8
    max7219_draw_image_8x8(
        &max7219_device, // pointer to max7219_t structure
        0,               // start line of 8x8 
        data_8x8         // data to display
    );


    // periodically change brightness to create a heartbit 
    while(true) {
        for (int i = 0; i < MAX7219_MAX_BRIGHTNESS; i++) {
            max7219_set_brightness(&max7219_device, i);
            vTaskDelay(1000 / MAX7219_MAX_BRIGHTNESS / portTICK_PERIOD_MS);
        }

        for (int i = MAX7219_MAX_BRIGHTNESS - 1; i > 0; i--) {
            max7219_set_brightness(&max7219_device, i);
            vTaskDelay(1000 / MAX7219_MAX_BRIGHTNESS / portTICK_PERIOD_MS);
        }

    }
}

