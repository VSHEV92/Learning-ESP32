#include "nimble/nimble_port.h"
#include "host/ble_hs.h"

#include "led_strip.h"

#define LED_GPIO 8

extern const struct ble_gatt_svc_def gatt_svcs[];
extern int gap_event_handler(struct ble_gap_event *event, void *arg);

led_strip_handle_t led_strip;

// NimBLE on_sync callback
void start_advertising(void) {

    struct ble_hs_adv_fields adv_fields = {0};

    // set advertising packet data. send only device name
    const char* adv_data = "NimBLE RGB Led Server";
    adv_fields.name = (uint8_t *)adv_data;
    adv_fields.name_len = strlen(adv_data);
    adv_fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&adv_fields);

    struct ble_gap_adv_params adv_params = {
        .conn_mode = BLE_GAP_CONN_MODE_UND,   // connetable undirect mode
        .disc_mode = BLE_GAP_DISC_MODE_NON,   // no scan response
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
        gap_event_handler,      // gap callback
        NULL                    // gap callback optional argument
    );

}

void app_main(void) {
    // ----------------------------------------
    // -------------- INIT LED ----------------
    // ----------------------------------------
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO,
        .max_leds = 1
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    
    led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);
    led_strip_clear(led_strip);

    led_strip_set_pixel(led_strip, 0, 0xff, 0, 0);
    led_strip_refresh(led_strip);

    // ----------------------------------------
    // --------------- INIT BT ----------------
    // ----------------------------------------

    // Set NimBLE stack sync callback
    ble_hs_cfg.sync_cb = start_advertising;
    
    // NimBLE host stack initialization
    nimble_port_init();

    // add ADC service to gatt 
    ble_gatts_count_cfg(gatt_svcs);
    ble_gatts_add_svcs(gatt_svcs);

    // run nimble stack
    nimble_port_run();

}
