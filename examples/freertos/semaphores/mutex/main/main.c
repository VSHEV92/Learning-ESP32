#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define TASK_STACK_SIZE 4096

static const char* TAG = "freertos_example";

static SemaphoreHandle_t semaphr;

void low_priority_task(void* param);
void high_priority_task(void* param);

void app_main(void) {
    BaseType_t status;

    // create and init mutex 
    ESP_LOGI(TAG, "Create mutex");
    semaphr = xSemaphoreCreateMutex();
   // xSemaphoreGive(semaphr);


    if (semaphr == NULL) {
        ESP_LOGE(TAG, "Can't create semaphore!");
        abort();
    }

    // create low priority task
    status = xTaskCreate(
        low_priority_task,
        "low_prior_task",
        TASK_STACK_SIZE,
        NULL,
        1,
        NULL
    );

    if (status != pdPASS){
        ESP_LOGE(TAG, "Can't create low priority task!");
        abort();
    }

    // create hight priority task
    status = xTaskCreate(
        high_priority_task,
        "high_prior_task",
        TASK_STACK_SIZE,
        NULL,
        2,
        NULL
    );

    if (status != pdPASS){
        ESP_LOGE(TAG, "Can't create high priority task!");
        abort();
    }

}

void low_priority_task(void* param) {
    while(true) {
        xSemaphoreTake(semaphr, portMAX_DELAY);
        vTaskDelay( pdMS_TO_TICKS(1000) );
        ESP_LOGI( pcTaskGetName(NULL) , "Current priority: %d", uxTaskPriorityGet(NULL));
        xSemaphoreGive(semaphr);
    }
}

void high_priority_task(void* param) {
    TaskHandle_t mutex_holder;
    while(true) {
        vTaskDelay( pdMS_TO_TICKS(100) );

        mutex_holder = xSemaphoreGetMutexHolder(semaphr);
        if(mutex_holder != NULL) {
            ESP_LOGI( pcTaskGetName(NULL), "Mutes is taken by %s", pcTaskGetName(mutex_holder));
        }
        
        xSemaphoreTake(semaphr, portMAX_DELAY);
        xSemaphoreGive(semaphr);
    }
}


