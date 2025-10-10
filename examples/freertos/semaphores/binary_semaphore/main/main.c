#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define TASK_STACK_SIZE 4096
#define SENDER_DELAY_MS 1000
#define SEMAPHORE_GIVES 5

static const char* TAG = "freertos_example";

static SemaphoreHandle_t semaphr;

void sender_task(void* param);
void receiver_task(void* param);

void app_main(void) {
    BaseType_t status;

    // create binary semaphore
    ESP_LOGI(TAG, "Create binary semaphore");

    semaphr = xSemaphoreCreateBinary();


    if (semaphr == NULL) {
        ESP_LOGE(TAG, "Can't create semaphore!");
        abort();
    }

    // create sender task
    status = xTaskCreate(
        sender_task,
        "sender_task",
        TASK_STACK_SIZE,
        NULL,
        1,
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
        2,
        NULL
    );

    if (status != pdPASS){
        ESP_LOGE(TAG, "Can't create receiver task!");
        abort();
    }
    
    // wait all semaphore action and delete semaphore
    vTaskDelay( pdMS_TO_TICKS(SENDER_DELAY_MS + 1000) * SEMAPHORE_GIVES );
    ESP_LOGI(TAG, "Delete semaphore");
    vSemaphoreDelete(semaphr);

}

void sender_task(void* param) {
    for (int i = 0; i < SEMAPHORE_GIVES; i++) {
        vTaskDelay( pdMS_TO_TICKS(SENDER_DELAY_MS) );
        ESP_LOGI("Sender", "Semapore give");
        xSemaphoreGive(semaphr);
    }

    vTaskDelete(NULL);
}

void receiver_task(void* param) {
    
    for (int i = 0; i < SEMAPHORE_GIVES; i++) {
        xSemaphoreTake(semaphr, portMAX_DELAY);
        ESP_LOGI("Receiver", "Semaphore taken");
    }

    vTaskDelete(NULL);
}


