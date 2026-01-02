#include "esp_log.h"
#include "host/ble_hs.h"

static const char TAG[] = "Nimble ADC Server";

extern void start_advertising(void);

extern bool adc_chr_ind_status;
extern uint16_t adc_chr_conn_handle;

// gap event handler
int gap_event_handler(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
        // Connect event
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI(TAG, "Connection %s", event->connect.status == 0 ? "established" : "failed");
            if (event->connect.status != 0) {
                start_advertising();
            }
            return 0;

        // Disconnect event
        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI(TAG, "Disconnected. Reason=%d", event->disconnect.reason);
            start_advertising();
            return 0;

        // Subscribe event
        case BLE_GAP_EVENT_SUBSCRIBE:
            ESP_LOGI(TAG, "Subscribe event. Indiation %s", event->subscribe.cur_indicate == 0 ? "disabled" : "enabled");
            adc_chr_ind_status = event->subscribe.cur_indicate;
            adc_chr_conn_handle = event->subscribe.conn_handle;
            return 0;
    }

    return 0;
}
