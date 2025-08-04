#ifndef LED_MATRIX_MAX7219_H
#define LED_MATRIX_MAX7219_H

#include <string.h>
#include <driver/spi_master.h>

#define MAX7219_HOST SPI2_HOST
#define MAX7219_CLK_FREQ 10000000

#define MAX7219_ADDR_SIZE 8
#define MAX7219_DATA_SIZE 8

#define MAX7219_DATA0_REG         0x1 
#define MAX7219_DATA1_REG         0x2 
#define MAX7219_DATA2_REG         0x3 
#define MAX7219_DATA3_REG         0x4 
#define MAX7219_DATA4_REG         0x5 
#define MAX7219_DATA5_REG         0x6 
#define MAX7219_DATA6_REG         0x7 
#define MAX7219_DATA7_REG         0x8 
#define MAX7219_SHUTDOWN_REG      0xC
#define MAX7219_DECODE_MODE_REG   0x9
#define MAX7219_INTENSITY_REG     0xA
#define MAX7219_SCAN_LIMIT_REG    0xB
#define MAX7219_DISPLAY_TEST_REG  0xF

extern uint8_t MAX7219_DATA_REGS [];

typedef struct {
    uint8_t mosi;
    uint8_t sclk;
    uint8_t cs;
    uint8_t decode_mask;
    uint8_t intensity;
    uint8_t scan_limit;
} max7219_config_t;


spi_device_handle_t max7219_init(max7219_config_t* cfg);

void max7219_send(spi_device_handle_t spi_dev, uint8_t addr, uint8_t data);

void max7219_shutdown_enable(spi_device_handle_t spi_dev);
void max7219_shutdown_disable(spi_device_handle_t spi_dev);

void max7219_display_test_enable(spi_device_handle_t spi_dev);
void max7219_display_test_disable(spi_device_handle_t spi_dev);

void max7219_scan_limit_set(spi_device_handle_t spi_dev, uint8_t limit);

void max7219_decode_mask_set(spi_device_handle_t spi_dev, uint8_t mask);

void max7219_intensity_set(spi_device_handle_t spi_dev, uint8_t intensity);

#endif
