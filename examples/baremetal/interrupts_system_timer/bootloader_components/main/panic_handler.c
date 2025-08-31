#include "esp_log.h"

static const char *TAG = "PANIC";

void panicHandler(void *frame) {
    ESP_EARLY_LOGI(TAG, "Inside ISR panic handler");
    while(1);
}

void xt_unhandled_exception(void *frame) {
    ESP_EARLY_LOGI(TAG, "Inside exception panic handler");
    while(1);
}

