#include "ess_server_gap.h"

// Initialize GAP service
void ess_server_gap_init(void) {
    // Set NimBLE stack sync callback
    ble_hs_cfg.sync_cb = ess_server_gap_start_advertising;
    //
    // GAP service initialization
    ble_svc_gap_init();
    ble_svc_gap_device_name_set(ESS_SERVER_GAP_DEVICE_NAME);
    ble_svc_gap_device_appearance_set(ESS_SERVER_GAP_DEVICE_APPEARANCE);
}

// NimBLE on_sync callback
void ess_server_gap_start_advertising(void) {

    struct ble_hs_adv_fields adv_fields = {0};

    // set advertising packet data
    adv_fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    adv_fields.le_role = ESS_SERVER_GAP_LE_ROLE_PERIPHERAL;
    adv_fields.le_role_is_present = 1;

    const char* device_name = ble_svc_gap_device_name();
    adv_fields.name = (uint8_t *)device_name;
    adv_fields.name_len = strlen(device_name);
    adv_fields.name_is_complete = 1;
    
    ble_gap_adv_set_fields(&adv_fields);
    

    struct ble_gap_adv_params adv_params = {
        .conn_mode = BLE_GAP_CONN_MODE_UND,   // connetable mode
        .disc_mode = BLE_GAP_DISC_MODE_GEN,   // general discoverable mode to have scan response
        .itvl_min = 0,                        // minimum advertising interval, if 0 stack use sane defaults
        .itvl_max = 0,                        // maximum advertising interval, if 0 stack use sane defaults
        .channel_map = 0,                     // advertising channel map, if 0 stack use sane defaults
        .filter_policy = 0,                   // advertising Filter policy
        .high_duty_cycle = false,             // if do High Duty cycle for Directed Advertising
    };

    ble_gap_adv_start(
        BLE_OWN_ADDR_PUBLIC,            // type of device address (public, random) 
        NULL,                           // peer address for direct advertising
        BLE_HS_FOREVER,                 // advertising duration, run gap callback in expire, BLE_HS_FOREVER - no timeout
        &adv_params,                    // advertising parameters
        ess_server_gap_event_handler,   // gap callback
        NULL                            // gap callback optional argument
    );

}

// GAP Events handler
int ess_server_gap_event_handler(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
        // Connect event
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI("", "Connection %s", event->connect.status == 0 ? "established" : "failed");
            if (event->connect.status != 0) {
                ess_server_gap_start_advertising();
            }
            return 0;

        // Disconnect event
        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI("", "Disconnected. Reason=%d", event->disconnect.reason);
            ess_server_gap_start_advertising();
            return 0;

    }

    return 0;
}
