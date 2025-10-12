#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define TASK_STACK_SIZE 4096

static const char* TAG = "freertos_example";

static EventGroupHandle_t event_group;

void sync_task(void* param);

void app_main(void) {
    BaseType_t status;

    // create event group
    ESP_LOGI(TAG, "Create event_group");

    event_group = xEventGroupCreate();


    if (event_group == NULL) {
        ESP_LOGE(TAG, "Can't create event group!");
        abort();
    }

    // create tasks
    status = xTaskCreate(
        sync_task,
        "sync_task_1",
        TASK_STACK_SIZE,
        (void *)0, // event sync flag
        1,
        NULL
    );
    if (status != pdPASS){
        ESP_LOGE(TAG, "Can't create sync task 1!");
        abort();
    }

    status = xTaskCreate(
        sync_task,
        "sync_task_2",
        TASK_STACK_SIZE,
        (void *)1, // event sync flag
        1,
        NULL
    );
    if (status != pdPASS){
        ESP_LOGE(TAG, "Can't create sync task 2!");
        abort();
    }

    status = xTaskCreate(
        sync_task,
        "sync_task_3",
        TASK_STACK_SIZE,
        (void *)2, // event sync flag
        1,
        NULL
    );
    if (status != pdPASS){
        ESP_LOGE(TAG, "Can't create sync task 3!");
        abort();
    }
    
}

void sync_task(void* param) {
    uint32_t sync_flag = (uint32_t) param;
    uint32_t delay_in_ticks = pdMS_TO_TICKS( 1000 + (sync_flag * 1333) );

    while(true) {
        vTaskDelay(delay_in_ticks);
        ESP_LOGI( pcTaskGetName(NULL) , "Move to sync barrier");
        xEventGroupSync(
            event_group,        // event group
            1 << sync_flag,     // flag to set
            0b111,              // flags to wait
            portMAX_DELAY       // wait timeout
        );
        ESP_LOGI( pcTaskGetName(NULL) , "Pass barrier");
    }

}



