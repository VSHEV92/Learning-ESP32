#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TASK_STACK_SIZE 4096
#define SENDER_DELAY_MS 1000

static const char* TAG = "freertos_example";

void sender_task(void* param);
void receiver_task(void* param);

void app_main(void) {
    BaseType_t status;

    // create sender task
    status = xTaskCreate(
        sender_task,
        "sender_task",
        TASK_STACK_SIZE,
        NULL,
        2,
        NULL
    );

    if (status != pdPASS){
        ESP_LOGE(TAG, "Can't create sender task!");
        abort();
    }

    // create receiver task
    status = xTaskCreate(
        receiver_task,
        "receiver_task",
        TASK_STACK_SIZE,
        NULL,
        1,
        NULL
    );

    if (status != pdPASS){
        ESP_LOGE(TAG, "Can't create receiver task!");
        abort();
    }
}

void sender_task(void* param) {
    // init sender delay
    vTaskDelay( pdMS_TO_TICKS(10) );

    TaskHandle_t receiver_task = xTaskGetHandle("receiver_task");

    vTaskDelay( pdMS_TO_TICKS(SENDER_DELAY_MS) );
    ESP_LOGI("Sender", "Notify index 1 two times using NotifyGive");

    for (int i = 0; i < 2; i++) {
        xTaskNotifyGiveIndexed( 
            receiver_task,  // task handle 
            1               // notify index
        );
    }

    vTaskDelay( pdMS_TO_TICKS(SENDER_DELAY_MS) );
    ESP_LOGI("Sender", "Notify index 2 three times using Notify");

    for (int i = 0; i < 3; i++) {
        xTaskNotifyIndexed( 
            receiver_task,  // task handle 
            2,              // notify index
            0,              // notify value (not used in this case)
            eIncrement      // action under notify value, increment notify value
        );
    }
    vTaskDelete(NULL);
}

void receiver_task(void* param) {
    
    ESP_LOGI("Receiver", "Wait two notifies for notify 1");
    for (int i = 0; i < 2; i++) {
        ulTaskNotifyTakeIndexed( 
            1,              // notify index
            pdFALSE,        // not clear value after get notification
            portMAX_DELAY   // max notify wait delay
        );
        ESP_LOGI("Receiver", "Get notification 1");
    }

    ESP_LOGI("Receiver", "Wait three notifies for notify 2");
    for (int i = 0; i < 3; i++) {
        ulTaskNotifyTakeIndexed( 
            2,              // notify index
            pdFALSE,        // not clear value after get notification
            portMAX_DELAY   // max notify wait delay
        );
        ESP_LOGI("Receiver", "Get notification 2");
    }

    vTaskDelete(NULL);
}


