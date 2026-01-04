#include "esp_log.h"
#include "host/ble_hs.h"

static const char TAG[] = "Nimble RGB Led Client";
static const char PEER_ADDRESS[] = "F0:F5:BD:27:59:6A";

extern void start_discovery(void); 

// gap event handler
int gap_event_handler(struct ble_gap_event *event, void *arg) {
    
    struct ble_hs_adv_fields fields;
    
    switch (event->type) {
        // --------------------------------------------------------
        // ----------------- Discovry event -----------------------
        case BLE_GAP_EVENT_DISC:
    
            uint32_t peer_addr[6];
            uint8_t test_addr[6];
            
            // get data from advertising packet
            ble_hs_adv_parse_fields(&fields, event->disc.data, event->disc.length_data);

            // check peer address
            memset(peer_addr, 0x0, sizeof peer_addr);
            
            sscanf(
                PEER_ADDRESS, "%lx:%lx:%lx:%lx:%lx:%lx",
                &peer_addr[5], &peer_addr[4], &peer_addr[3],
                &peer_addr[2], &peer_addr[1], &peer_addr[0]
            );

            for (int i=0; i<6; i++) {
               test_addr[i] = (uint8_t )peer_addr[i];
            }

	        if (memcmp(test_addr, event->disc.addr.val, sizeof(event->disc.addr.val)) != 0) {
                return 0;
            }
            
            ESP_LOGI(TAG, "Peer address matched: %s", PEER_ADDRESS);
    

            // The device has to be advertising connectability
            if (event->disc.event_type != BLE_HCI_ADV_RPT_EVTYPE_ADV_IND &&
                event->disc.event_type != BLE_HCI_ADV_RPT_EVTYPE_DIR_IND) {
                return 0;
            }
            ESP_LOGI(TAG, "Peer device is connectable");

            // Scanning must be stopped before a connection can be initiated.
            ble_gap_disc_cancel();
            ESP_LOGI(TAG, "Stop scanning");

            // Connect to peer
            ble_gap_connect(
                BLE_OWN_ADDR_PUBLIC,     // The type of address the stack should use for itself during connection establishment
                &(event->disc.addr),     // The address of the peer to connect to
                30000,                   // The duration of the discovery procedure. Units are milliseconds.  Specify BLE_HS_FOREVER for no expiration. Specify 0 to use stack defaults
                NULL,                    // Additional arguments specifying the particulars of the discovery procedure    
                gap_event_handler,       // The callback to associate with this discovery procedure
                NULL                     // The optional argument to pass to the callback function
            );

            return 0;

        // --------------------------------------------------------
        // ------------------- Connect event ----------------------
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI(TAG, "Connection %s", event->connect.status == 0 ? "established" : "failed");
            return 0;

        // --------------------------------------------------------
        // -------------------- Disconnect event ------------------
        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI(TAG, "Disconnected. Reason=%d", event->disconnect.reason);
            start_discovery();
            return 0;

    }

    return 0;
}
