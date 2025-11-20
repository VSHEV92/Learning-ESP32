#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define ADC_CONV_PER_FRAME   3
#define ADC_FRAMES_PER_POOL  3
#define ADC_CONV_FRAME_SIZE  (SOC_ADC_DIGI_RESULT_BYTES * ADC_CONV_PER_FRAME)
#define ADC_CONV_POOL_SIZE   (ADC_CONV_FRAME_SIZE * ADC_FRAMES_PER_POOL)
#define ADC_SAMPLE_RATE_HZ   1000

static const char *TAG = "ADC SAMPLE";

void app_main(void)
{
    // init adc handle
    adc_continuous_handle_cfg_t adc_handle_cfg = {
        .max_store_buf_size = ADC_CONV_POOL_SIZE,
        .conv_frame_size = ADC_CONV_FRAME_SIZE,
        .flags = {
            .flush_pool = false 
        }
    };

    adc_continuous_handle_t adc_handle;
    adc_continuous_new_handle(&adc_handle_cfg, &adc_handle);


    // setup adc convertion
    adc_continuous_config_t adc_conv_cfg = {
        .pattern_num = 1,
        .adc_pattern = NULL,
        .sample_freq_hz = ADC_SAMPLE_RATE_HZ,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,     // use ADC1 only
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,  // nly type2 supported for esp32c3
    };

    adc_digi_pattern_config_t adc_pattern = { 
        adc_pattern.atten = ADC_ATTEN_DB_12,
        adc_pattern.channel = ADC_CHANNEL_0,
        adc_pattern.unit = ADC_UNIT_1,
        adc_pattern.bit_width = SOC_ADC_DIGI_MAX_BITWIDTH // max supported bitwidth

    };
    adc_conv_cfg.adc_pattern = &adc_pattern;

    adc_continuous_config(adc_handle, &adc_conv_cfg);

    // configure calibration handle
    adc_cali_curve_fitting_config_t adc_cali_cfg = {
        .unit_id = ADC_UNIT_1,
        .chan = ADC_CHANNEL_0,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = SOC_ADC_DIGI_MAX_BITWIDTH,
    };

    adc_cali_handle_t adc_cali_handle;
    adc_cali_create_scheme_curve_fitting(&adc_cali_cfg, &adc_cali_handle);

    // start continuous adc convertion
    adc_continuous_start(adc_handle);

    // read sample and print them on the screen
    uint8_t adc_conv_buff[ADC_CONV_FRAME_SIZE];
    uint32_t readed_bytes_number;
    int adc_voltage_mV;

    while (true) {
        // get convertion frame buffer
        adc_continuous_read(
            adc_handle,           // adc handle 
            adc_conv_buff,        // buffer that contain convertion frame 
            ADC_CONV_FRAME_SIZE,  // buffer size in bytes
            &readed_bytes_number, // number of valid bytes in buffer after function return 
            ADC_MAX_DELAY         // timeout for convartion frame waiting (max value, infinite)
        );

        // stop adc convertion
        adc_continuous_stop(adc_handle);

        // parse convertion frame buffer and print all convertion results
        ESP_LOGI(TAG, "Convertion frame results: ");
        for (int i = 0; i < readed_bytes_number; i += SOC_ADC_DIGI_RESULT_BYTES) {
            adc_digi_output_data_t* conv_result = (adc_digi_output_data_t*)(&adc_conv_buff[i]);
            adc_cali_raw_to_voltage(adc_cali_handle, (int)(conv_result->type2.data), &adc_voltage_mV);
            ESP_LOGI(TAG, "Sample %d: %d mV", i/SOC_ADC_DIGI_RESULT_BYTES, adc_voltage_mV);
        }
        ESP_LOGI(TAG, "\n");

        // wait one 200ms and start next adc convertion
        vTaskDelay( pdMS_TO_TICKS(200) );
        adc_continuous_start(adc_handle);
    }
}
