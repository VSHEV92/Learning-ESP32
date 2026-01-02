#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "esp_adc/adc_oneshot.h"

#include "nimble/nimble_port.h"
#include "host/ble_hs.h"

#define STACK_SIZE 4096

static int adc_raw_data;
static adc_oneshot_unit_handle_t adc_handle;

// task for ADC processing
void process_adc(void* paramenter) {
    struct ble_hs_adv_fields scan_fields = {0};

    while(1) {
        adc_oneshot_read(adc_handle, ADC_CHANNEL_0, &adc_raw_data);
        ESP_LOGI("Nimble ADC Broadcaster", "ADC raw data is %x", adc_raw_data);
        vTaskDelay( pdMS_TO_TICKS(200) );

        // set scan response data. send adc raw data
        scan_fields.mfg_data = (uint8_t *)(&adc_raw_data);
        scan_fields.mfg_data_len = sizeof(adc_raw_data);
        ble_gap_adv_rsp_set_fields(&scan_fields);
    }
}

// task, that start NimBLE host processing
void process_bt(void* paramenter) {
    nimble_port_run();
}

// NimBLE on_sync callback
void start_advertising(void) {

    struct ble_hs_adv_fields adv_fields = {0};
    struct ble_hs_adv_fields scan_fields = {0};

    // set advertising packet data. send only device name
    const char* adv_data = "NimBLE ADC Broadcaster";
    adv_fields.name = (uint8_t *)adv_data;
    adv_fields.name_len = strlen(adv_data);
    adv_fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&adv_fields);
    
    // set scan response data. send adc raw data
    scan_fields.mfg_data = (uint8_t *)(&adc_raw_data);
    scan_fields.mfg_data_len = sizeof(adc_raw_data);
    ble_gap_adv_rsp_set_fields(&scan_fields);

    struct ble_gap_adv_params adv_params = {
        .conn_mode = BLE_GAP_CONN_MODE_NON,   // non-connetable mode a beacon
        .disc_mode = BLE_GAP_DISC_MODE_GEN,   // general discoverable mode to have scan response
        .itvl_min = 0,                        // minimum advertising interval, if 0 stack use sane defaults
        .itvl_max = 0,                        // maximum advertising interval, if 0 stack use sane defaults
        .channel_map = 0,                     // advertising channel map, if 0 stack use sane defaults
        .filter_policy = 0,                   // advertising Filter policy
        .high_duty_cycle = false,             // if do High Duty cycle for Directed Advertising
    };

    ble_gap_adv_start(
        BLE_OWN_ADDR_PUBLIC,    // type of device address (public, random) 
        NULL,                   // peer address for direct advertising
        BLE_HS_FOREVER,         // advertising duration, run gap callback in expire, BLE_HS_FOREVER - no timeout
        &adv_params,            // advertising parameters
        NULL,                   // gap callback
        NULL                    // gap callback optional argument
    );

}

void app_main(void) {
    // ----------------------------------------
    // -------------- INIT ADC ----------------
    // ----------------------------------------

    // configure ADC unit
    adc_oneshot_unit_init_cfg_t adc_unit_cfg = {
        .unit_id = ADC_UNIT_1,
        .clk_src = ADC_DIGI_CLK_SRC_APB,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };

    adc_oneshot_new_unit(&adc_unit_cfg, &adc_handle);

    // configure ADC channel
    adc_oneshot_chan_cfg_t adc_channel_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT, // max supported bitwidth
        .atten = ADC_ATTEN_DB_12,
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_0, &adc_channel_cfg);


    // ----------------------------------------
    // --------------- INIT BT ----------------
    // ----------------------------------------

    // Set NimBLE stack sync callback
    ble_hs_cfg.sync_cb = start_advertising;
    
    // NimBLE host stack initialization
    nimble_port_init();
    

    // ----------------------------------------
    // --------- START FREERTOS TASKS ---------
    // ----------------------------------------
    xTaskCreate(
        process_adc,      // called function
        "adc_processing", // human readable name
        STACK_SIZE,       // stack size
        NULL,             // task function parameter (void*)
        1,                // task priority
        NULL              // task handler
    );

    xTaskCreate(
        process_bt,       // called function
        "bt_processing",  // human readable name
        STACK_SIZE,       // stack size
        NULL,             // task function parameter (void*)
        1,                // task priority
        NULL              // task handler
    );

}
