#include "nimble/nimble_port.h"
#include "host/ble_hs.h"

//extern const struct ble_gatt_svc_def gatt_svcs[];
extern int gap_event_handler(struct ble_gap_event *event, void *arg);


// NimBLE on_sync callback
void start_discovery(void) {

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
        BLE_OWN_ADDR_PUBLIC,   // The type of address the stack should use for itself when sending scan requests
        BLE_HS_FOREVER,        // The duration of the discovery procedure. Units are milliseconds.  Specify BLE_HS_FOREVER for no expiration. Specify 0 to use stack defaults
        &disc_params,          // Additional arguments specifying the particulars of the discovery procedure    
        gap_event_handler,     // The callback to associate with this discovery procedure
        NULL                   // The optional argument to pass to the callback function
    );

}

void app_main(void) {

    // ----------------------------------------
    // --------------- INIT BT ----------------
    // ----------------------------------------

    // Set NimBLE stack sync callback
    ble_hs_cfg.sync_cb = start_discovery;
    
    // NimBLE host stack initialization
    nimble_port_init();

    // run nimble stack
    nimble_port_run();

}
