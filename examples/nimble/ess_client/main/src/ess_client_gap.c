#include "ess_client_gap.h"

static uint16_t ess_server_connection_handle = 0;
static uint16_t ess_server_chr_temp_celsius_handle = 0;

static const ble_uuid16_t ess_uuid = BLE_UUID16_INIT(ESS_CLIENT_GAP_UUID_ESS);
static const ble_uuid16_t ess_chr_temp_celsius_uuid = BLE_UUID16_INIT(ESS_CLIENT_GAP_UUID_TEMP_CELSIUS);

static int gap_event_handler(struct ble_gap_event *event, void *arg);
static int service_discovered_cb(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_svc *svc, void *arg);
static int chr_discovered_cb(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_chr *chr, void *arg);


// Initialize GAP service
void ess_client_gap_init(void) {
    // Set NimBLE stack sync callback
    ble_hs_cfg.sync_cb = ess_client_gap_start_discovery;
    //
    // GAP service initialization
    ble_svc_gap_init();
    ble_svc_gap_device_name_set(ESS_CLIENT_GAP_DEVICE_NAME);
    ble_svc_gap_device_appearance_set(ESS_CLIENT_GAP_DEVICE_APPEARANCE);
}

// NimBLE on_sync callback
void ess_client_gap_start_discovery(void) {

    struct ble_gap_disc_params disc_params = {
        .itvl = 0,                  // Scan interval in 0.625ms units
        .window = 0,                // Scan window in 0.625ms units
        .filter_policy = 0,         // Scan filter policy
        .limited = 0,               // If limited discovery procedure should be used
        .passive = 1,               // If passive scan should be used
        .filter_duplicates = 1,     // If enable duplicates filtering
        .disable_observer_mode = 0, // If observation procedure is to be disabled. By default all advertisement reports are accepted
    };

    ble_gap_disc (
        BLE_OWN_ADDR_PUBLIC,            // The type of address the stack should use for itself when sending scan requests
        BLE_HS_FOREVER,                 // The duration of the discovery procedure. Units are milliseconds.  Specify BLE_HS_FOREVER for no expiration. Specify 0 to use stack defaults
        &disc_params,                   // Additional arguments specifying the particulars of the discovery procedure    
        gap_event_handler,              // The callback to associate with this discovery procedure
        NULL                            // The optional argument to pass to the callback function
    );
}

// GAP Events handler
static int gap_event_handler(struct ble_gap_event *event, void *arg) {
    struct ble_hs_adv_fields fields;
    char server_name[] = "esp32_ess_example";

    switch (event->type) {
        // --------------------------------------------------------
        // ----------------- Discovry event -----------------------
        case BLE_GAP_EVENT_DISC:
            
            // get data from advertising packet
            ble_hs_adv_parse_fields(&fields, event->disc.data, event->disc.length_data);

            // advertising data must have a concrete name 
            if (fields.name_len == 0) {
                return 0;
            }
            else if ( strncmp((char*)fields.name, server_name, fields.name_len) ) {
                return 0;
            }
            else {
                ESP_LOGI("", "Find ESS Server Device %s", server_name);
            }


            // The device has to be advertising connectability
            if (event->disc.event_type != BLE_HCI_ADV_RPT_EVTYPE_ADV_IND &&
                event->disc.event_type != BLE_HCI_ADV_RPT_EVTYPE_DIR_IND) {
                return 0;
            }
            ESP_LOGI("", "Peer device is connectable");

            // Scanning must be stopped before a connection can be initiated.
            ble_gap_disc_cancel();
            ESP_LOGI("", "Stop scanning");

            // Connect to peer
            ble_gap_connect(
                BLE_OWN_ADDR_PUBLIC,                // The type of address the stack should use for itself during connection establishment
                &(event->disc.addr),                // The address of the peer to connect to
                30000,                              // The duration of the discovery procedure. Units are milliseconds.  Specify BLE_HS_FOREVER for no expiration. Specify 0 to use stack defaults
                NULL,                               // Additional arguments specifying the particulars of the discovery procedure    
                gap_event_handler,                  // The callback to associate with this discovery procedure
                NULL                                // The optional argument to pass to the callback function
            );

            return 0;

        // --------------------------------------------------------
        // ------------------- Connect event ----------------------
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI("", "Connection %s, Handle: %d", event->connect.status == 0 ? "established" : "failed", event->connect.conn_handle);

            // Start services discovery
            ble_gattc_disc_svc_by_uuid(event->connect.conn_handle, &ess_uuid.u, service_discovered_cb, NULL);

            return 0;

        // --------------------------------------------------------
        // -------------------- Disconnect event ------------------
        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI("", "Disconnected. Reason=%d", event->disconnect.reason);
            ess_server_connection_handle = 0;
            ess_server_chr_temp_celsius_handle = 0;
            ess_client_led_matrix_stop_read();
            ess_client_gap_start_discovery();
            return 0;
    }
    return 0;
}

static int service_discovered_cb(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_svc *svc, void *arg) {
    if (error->status == 0) {
        if (svc == NULL) { return 0; }
        ESP_LOGI("", "Find ESS. Handles: start = %d, end = %d", svc->start_handle, svc->end_handle);
        ble_gattc_disc_all_chrs(conn_handle, svc->start_handle, svc->end_handle, chr_discovered_cb, NULL);
    }
    return 0;
}

static int chr_discovered_cb(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_chr *chr, void *arg) {
    if (error->status == 0) {
        if (ble_uuid_cmp(&chr->uuid.u, &ess_chr_temp_celsius_uuid.u) == 0) {
            ESP_LOGI("", "Find ESS Temperature Characteristic. Handle: %d", chr->val_handle);
            ess_server_connection_handle = conn_handle;
            ess_server_chr_temp_celsius_handle = chr->val_handle;

            // Start temperature reading
            ESP_LOGI("", "Start temperature reading");
            ess_client_led_matrix_start_read(ess_server_connection_handle, ess_server_chr_temp_celsius_handle);
        }
    }
    return 0;
}


