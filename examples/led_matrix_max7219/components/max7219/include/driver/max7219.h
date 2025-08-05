#ifndef MAX7219_H
#define MAX7219_H

#include <string.h>
#include <driver/spi_master.h>

#define MAX7219_HOST SPI2_HOST
#define MAX7219_CLK_FREQ 10000000

#define MAX7219_ADDR_SIZE 8
#define MAX7219_DATA_SIZE 8

#define MAX7219_INTENSITY_LEVELS 16

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


/**
 * MAX7219 Configuration structure
 */
typedef struct {
    uint8_t mosi;           // MOSI GPIO Number
    uint8_t sclk;           // SCLK GPIO Number
    uint8_t cs;             // CS GPIO Number
    uint8_t decode_mask;    // Initial decode mask
    uint8_t intensity;      // Initial intensity
    uint8_t scan_limit;     // Initial Scan-Limit
} max7219_config_t;


/**
 * @brief Initialize max7219 device
 *
 * @param cfg MAX7219 configuration structure
 * @return spi_device_handler
 */
spi_device_handle_t max7219_init(max7219_config_t* cfg);


/**
 * @brief Send data max7219 device
 *
 * @param spi_dev SPI device handler
 * @param addr register address
 * @param data register value
 * @return void
 */
void max7219_send(spi_device_handle_t spi_dev, uint8_t addr, uint8_t data);


/**
 * @brief Enable shutdown
 *
 * @param spi_dev SPI device handler
 * @return void
 */
void max7219_shutdown_enable(spi_device_handle_t spi_dev);


/**
 * @brief Disable shutdown
 *
 * @param spi_dev SPI device handler
 * @return void
 */
void max7219_shutdown_disable(spi_device_handle_t spi_dev);


/**
 * @brief Enable display test mode
 *
 * @param spi_dev SPI device handler
 * @return void
 */
void max7219_display_test_enable(spi_device_handle_t spi_dev);


/**
 * @brief Disable display test mode
 *
 * @param spi_dev SPI device handler
 * @return void
 */
void max7219_display_test_disable(spi_device_handle_t spi_dev);


/**
 * @brief Set scan limit
 *
 * @param spi_dev SPI device handler
 * @param limit Scan-Limit value
 * @return void
 */
void max7219_scan_limit_set(spi_device_handle_t spi_dev, uint8_t limit);


/**
 * @brief Set decode mask
 *
 * @param spi_dev SPI device handler
 * @param mask decode mask value
 * @return void
 */
void max7219_decode_mask_set(spi_device_handle_t spi_dev, uint8_t mask);


/**
 * @brief Set intensity
 *
 * @param spi_dev SPI device handler
 * @param intensity intensity value
 * @return void
 */
void max7219_intensity_set(spi_device_handle_t spi_dev, uint8_t intensity);

#endif
