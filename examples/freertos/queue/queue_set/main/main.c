#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define TASK_STACK_SIZE 4096
#define SENDER_DELAY_MS 1000
#define QUEUE_LEN 5
#define QUEUES_NUMBER 3

static const char* TAG = "freertos_example";


static QueueSetHandle_t queue_set;
static QueueHandle_t queues[QUEUES_NUMBER];
static char queues_names[QUEUES_NUMBER][40];

void sender_task(void* param);
void receiver_task(void* param);

void app_main(void) {
    BaseType_t status;

    // create queue set
    queue_set = xQueueCreateSet (QUEUES_NUMBER * QUEUE_LEN);

    if (queue_set == NULL) {
        ESP_LOGE(TAG, "Can't create queue set!");
        abort();
    }

    // create queues
    ESP_LOGI(TAG, "Create queues");

    for (int i = 0; i < QUEUES_NUMBER; i++) {
        queues[i] = xQueueCreate( QUEUE_LEN, sizeof(uint32_t) );

        if (queues[i] == NULL) {
            ESP_LOGE(TAG, "Can't create queue %d!", i);
            abort();
        }
        
        sprintf(queues_names[i], "queue_%d", i);
        vQueueAddToRegistry(queues[i], queues_names[i]);

        xQueueAddToSet(queues[i], queue_set);
    }

    // create sender task
    for (int i = 0; i < QUEUES_NUMBER; i++) {

        status = xTaskCreate(
            sender_task,
            NULL,
            TASK_STACK_SIZE,
            (void*) i,
            1,
            NULL
        );

        if (status != pdPASS){
            ESP_LOGE(TAG, "Can't create sender task %d!", i);
            abort();
        }
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
    int queue_idx = (int)param;
    uint32_t data_to_send = queue_idx;
    
    while(true) {
        ESP_LOGI("Sender", "Send %u value to queue %d", (unsigned int)data_to_send, queue_idx);
        xQueueSendToBack(queues[queue_idx], (void*)&data_to_send, portMAX_DELAY);
        data_to_send += 10;
        vTaskDelay( pdMS_TO_TICKS(1000 + (333 * queue_idx)) );
    }
}

void receiver_task(void* param) {

    QueueHandle_t queue;

    uint32_t data_received;
    
    while(true) {
        queue = xQueueSelectFromSet(queue_set, portMAX_DELAY);
        xQueueReceive(queue, &data_received, portMAX_DELAY);
        ESP_LOGI("Receiver", "Receive %u value from %s", (unsigned int)data_received, pcQueueGetName(queue));
    }

}


