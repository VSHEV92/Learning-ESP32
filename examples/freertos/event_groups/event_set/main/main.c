#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define TASK_STACK_SIZE 4096

static const char* TAG = "freertos_example";

static EventGroupHandle_t event_group;

void sender_task(void* param);
void receiver_task(void* param);

void app_main(void) {
    BaseType_t status;

    // create event group
    ESP_LOGI(TAG, "Create event_group");

    event_group = xEventGroupCreate();


    if (event_group == NULL) {
        ESP_LOGE(TAG, "Can't create event group!");
        abort();
    }

    // create sender task
    status = xTaskCreate(
        sender_task,
        "send_task_1",
        TASK_STACK_SIZE,
        (void *)0, // evant flag to set 
        1,
        NULL
    );

    if (status != pdPASS){
        ESP_LOGE(TAG, "Can't create sender task 1!");
        abort();
    }

    status = xTaskCreate(
        sender_task,
        "send_task_2",
        TASK_STACK_SIZE,
        (void*)1,  // evant flag to set 
        1,
        NULL
    );

    if (status != pdPASS){
        ESP_LOGE(TAG, "Can't create sender task 1!");
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
    uint32_t flag_to_set = (uint32_t) param;
    uint32_t delay_in_ticks = pdMS_TO_TICKS( 1000 + (flag_to_set * 133) );

    while(true) {
        vTaskDelay(delay_in_ticks);
        ESP_LOGI( pcTaskGetName(NULL) , "Set event flag %u", (unsigned int)flag_to_set);
        xEventGroupSetBits(event_group, 1 << flag_to_set);
    }

}

void receiver_task(void* param) {
    
    while(true) {
        ESP_LOGI("Receiver", "Wait for event bits 0 and 1");
        
        xEventGroupWaitBits(
            event_group,   // event group 
            0b11,          // bits to wait
            pdTRUE,        // clear bits after wait
            pdTRUE,        // wait all bits (logic and)
            portMAX_DELAY  // max ticks to wait 
        );

        ESP_LOGI("Receiver", "Event bits 0 and 1 received");
    }

}


