#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TASK_STACK_SIZE 4096

static const char* TAG = "freertos examples";

void task_1 (void* param);
void task_2 (void* param);

void app_main(void) {
    BaseType_t status;

    // create first task
    status = xTaskCreate(
        task_1,
        "task_1",
        TASK_STACK_SIZE,
        NULL,
        1,
        NULL
    );

    // check if first task created successfully
    if (status != pdPASS){
        ESP_LOGE(TAG, "Can't create task_1!");
        abort();
    }

    // create second task
    status = xTaskCreate(
        task_2,
        "task_2",
        TASK_STACK_SIZE,
        NULL,
        2,
        NULL
    );

    // check if second task created successfully
    if (status != pdPASS){
        ESP_LOGE(TAG, "Can't create task_2!");
        abort();
    }
}


// task 1 get task 2 handler by it's name
// it wait for 5 seconds and resume task 3
void task_1 (void* param) {

    // wait 1 second, so task_2 is guaranteed started 
    vTaskDelay( pdMS_TO_TICKS(1000) );

    // get task_2 handle
    TaskHandle_t task_2_handle = xTaskGetHandle("task_2");
    if (task_2_handle == NULL) {
        ESP_LOGE(TAG, "Can't get task_2 handle!");
        abort();
    }
    
    while(true){
        vTaskDelay( pdMS_TO_TICKS(5000) );
        ESP_LOGI(TAG, "Task 1 resume task 2");
        vTaskResume(task_2_handle);
    }

}


// task 2 print some messages and then suspend it self
void task_2 (void* param) {
    
    while(true) {
        ESP_LOGI(TAG, "Task 2 is running");
        vTaskDelay( pdMS_TO_TICKS(2000) );
        ESP_LOGI(TAG, "Task 2 suspend it self");
        vTaskSuspend(NULL);
    }

}
