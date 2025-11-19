#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
//#include "esp_adc/adc_cali_scheme.h"

const static char *TAG = "ADC ONESHOT";

void app_main(void)
{
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
        .bitwidth = ADC_BITWIDTH_DEFAULT, // max supported bitwidth
        .atten = ADC_ATTEN_DB_12,
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_0, &adc_channel_cfg);

    // configure calibration handle
    adc_cali_curve_fitting_config_t adc_cali_cfg = {
        .unit_id = ADC_UNIT_1,
        .chan = ADC_CHANNEL_0,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    adc_cali_handle_t adc_cali_handle;
    adc_cali_create_scheme_curve_fitting(&adc_cali_cfg, &adc_cali_handle);

    // ------------------------------------------------------------------------------------------
    int adc_raw_data;
    int adc_voltage_mV;
    
    while (true) {
        // read adc raw data (bits directly for ADC)
        adc_oneshot_read(adc_handle, ADC_CHANNEL_0, &adc_raw_data);

        // convert ADC raw data to mV
        adc_cali_raw_to_voltage(adc_cali_handle, adc_raw_data, &adc_voltage_mV);

        ESP_LOGI(TAG, "ADC valtage is %u mV", adc_voltage_mV);
        vTaskDelay( pdMS_TO_TICKS(200) );

    }
}
