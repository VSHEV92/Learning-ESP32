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
        "sender_task_1",
        TASK_STACK_SIZE,
        (void*)0,
        1,
        NULL
    );

    if (status != pdPASS){
        ESP_LOGE(TAG, "Can't create sender task 1!");
        abort();
    }

    status = xTaskCreate(
        sender_task,
        "sender_task_2",
        TASK_STACK_SIZE,
        (void*)1,
        1,
        NULL
    );

    if (status != pdPASS){
        ESP_LOGE(TAG, "Can't create sender task 2!");
        abort();
    }

    // create receiver task
    status = xTaskCreate(
        receiver_task,
        "receiver_task",
        TASK_STACK_SIZE,
        NULL,
        2,
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


    unsigned int event_flag = (1 << (unsigned int)param);
    TaskHandle_t receiver_task = xTaskGetHandle("receiver_task");

    while (true){
        vTaskDelay( pdMS_TO_TICKS(SENDER_DELAY_MS + (733 * (unsigned int)param)) );
        ESP_LOGI( pcTaskGetName(NULL), "Notify receiver by flag %u", event_flag);

        xTaskNotify( 
            receiver_task,  // task handle 
            event_flag,     // notify value
            eSetBits        // notify action 
        );
    }
}

void receiver_task(void* param) {
    uint32_t notify_value;

    while(true) {
        xTaskNotifyWait( 
            0x0,            // clear notify value under function entry
            0xf,            // clear notify value under function exit
            &notify_value,  // notify value
            portMAX_DELAY   // max notify wait delay
        );
        ESP_LOGI("Receiver", "Get notification from sender %u", (unsigned int)notify_value);
    }
    vTaskDelete(NULL);
}


