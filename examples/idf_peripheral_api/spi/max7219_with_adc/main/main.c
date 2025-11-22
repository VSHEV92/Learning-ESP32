#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "max7219.h"

#define MAX7219_SCLK 4
#define MAX7219_MOSI 2
#define MAX7219_CS 3

#define ADC_MAX_VALUE (1 << ADC_BITWIDTH_12)
#define MAX7219_LINES 8

#define COMPARE_THRESHOLD_STEP (ADC_MAX_VALUE / (MAX7219_LINES + 1))

void app_main(void) {

    // -----------------------------------------
    // ----------- configure adc ---------------
    // -----------------------------------------

    // configure ADC unit
    adc_oneshot_unit_init_cfg_t adc_unit_cfg = {
        .unit_id = ADC_UNIT_1,
        .clk_src = ADC_DIGI_CLK_SRC_APB,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };

    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_new_unit(&adc_unit_cfg, &adc_handle);

    // configure ADC channel
    adc_oneshot_chan_cfg_t adc_channel_cfg = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_0, &adc_channel_cfg);


    // -----------------------------------------
    // -------- configure max7219 device -------
    // -----------------------------------------

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
    max7219_init_desc(&max7219_device, SPI2_HOST, MAX7219_MAX_CLOCK_SPEED_HZ, MAX7219_CS);
    max7219_init(&max7219_device);


    // -----------------------------------------
    // -------- display signal intensity -------
    // -----------------------------------------

    // calculate threshold value
    int compare_thresholds[MAX7219_LINES];
    for (int i = 0; i < MAX7219_LINES; i++){
        compare_thresholds[i] = (i+1) * COMPARE_THRESHOLD_STEP;
    }

    // read adc raw data, compare with thresholds and display on the max7219 
    while (true) {
        int adc_raw_data;
        adc_oneshot_read(adc_handle, ADC_CHANNEL_0, &adc_raw_data);

        for (int i = 0; i < MAX7219_LINES; i++){
            int line_value = (adc_raw_data > compare_thresholds[i]) ? 0xFF : 0x0;
            max7219_set_digit(&max7219_device, i, line_value);
        }
    }

}

