#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define TASK_STACK_SIZE 4096
#define SENDER_DELAY_MS 1000
#define QUEUE_LEN 5

static const char* TAG = "freertos_example";

static QueueHandle_t queue;

void sender_task(void* param);
void receiver_task(void* param);

void app_main(void) {
    BaseType_t status;

    // create queue
    ESP_LOGI(TAG, "Create queue for uint32_t lendth of %d", QUEUE_LEN);

    queue = xQueueCreate( QUEUE_LEN,          // number of items in queue 
                          sizeof(uint32_t) ); // size of one item in bytes


    if (queue == NULL) {
        ESP_LOGE(TAG, "Can't create queue!");
        abort();
    }

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
    uint32_t data_to_send = 0;
    
    for (int i = 0; i < QUEUE_LEN; i++) {
        ESP_LOGI("Sender", "Send %u value to queue", (unsigned int)data_to_send);
        xQueueSendToFront(queue, (void*)&data_to_send, portMAX_DELAY);
        data_to_send += 10;
    }

    vTaskDelete(NULL);
}

void receiver_task(void* param) {

    uint32_t data_received;
    
    for (int i = 0; i < QUEUE_LEN; i++) {
        xQueueReceive(queue, &data_received, portMAX_DELAY);
        ESP_LOGI("Receiver", "Receive %u value from queue", (unsigned int)data_received);
    }

    vTaskDelete(NULL);
}


