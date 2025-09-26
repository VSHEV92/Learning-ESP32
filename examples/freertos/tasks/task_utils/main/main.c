#include "stdio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define NUMBER_OF_TASKS 5
#define TASK_STACK_SIZE 4096
#define TASK_STATS_BUFFER_SIZE (40 * (NUMBER_OF_TASKS + 5) ) 

static const char* TAG = "task_examples";

void task_function(void* param);

void app_main(void) {
    char task_statisics[TASK_STATS_BUFFER_SIZE];
    char task_name[40];
    BaseType_t status;
    TaskHandle_t task_handle;

    // create several tasks
    for (int i = 0; i < NUMBER_OF_TASKS; i++) {
        // create task with different name
        sprintf(task_name, "task_%d", i),

        status = xTaskCreate(
            task_function,
            task_name,
            TASK_STACK_SIZE,
            NULL,
            1,
            &task_handle
        );
        
        // check that task is created successfully
        if (status != pdPASS){
            ESP_LOGE(TAG, "Can't create task %d", i);
            abort();
        }

        // set task TAG
        vTaskSetApplicationTaskTag( task_handle, ( void * ) i );
    }

    // wait for 5 seconds
    vTaskDelay( pdMS_TO_TICKS(5000) );

    // print task statistics
    int number_of_tasks = uxTaskGetNumberOfTasks();
    ESP_LOGI(TAG, "Number of tasks is %d", number_of_tasks);

    vTaskList( task_statisics );
    ESP_LOGI(TAG, "List of tasks\n%s", task_statisics);
   
    vTaskGetRunTimeStats( task_statisics );
    ESP_LOGI(TAG, "Runtime statistics\n%s", task_statisics);

    // delete all tasks
    for (int i = 0; i < NUMBER_OF_TASKS; i++){
        sprintf(task_name, "task_%d", i),
        task_handle = xTaskGetHandle(task_name);
        vTaskDelete(task_handle);
    }

}

void task_function(void* param){
    char* task_name;
    char log_tag[10];

    // task startup delay
    vTaskDelay( pdMS_TO_TICKS(1000) );

    // get task tag
    int task_tag = ( int ) xTaskGetApplicationTaskTag(NULL);
    sprintf(log_tag, "task_%d", task_tag);
    
    while(true){
        // get task name
        task_name = pcTaskGetName(NULL);

        ESP_LOGI(log_tag, "Hello from %s", task_name);

        vTaskDelay( pdMS_TO_TICKS(1000) );
    }
}
