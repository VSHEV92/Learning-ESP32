#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define STACK_SIZE 1000

// task_1 - create using heap
// task_2 - static creation
TaskHandle_t task_1, task_2;

void task_1_function(void* paramenter);
void task_2_function(void* paramenter);

StackType_t task_2_stack[STACK_SIZE];
StaticTask_t task_2_TCB;

void app_main(void) {
    BaseType_t status;

    // -----------------------------------
    // ----- Create Task Using Heap ------
    // -----------------------------------

    // 1. Create first task using heap
    printf("Create task_1\n");

    status = xTaskCreate(
                    task_1_function,  // called function
                    "task_1",         // human readable name
                    STACK_SIZE,       // stack size
                    NULL,             // task function parameter (void*)
                    1,                // task priority
                    &task_1           // task handler
    );

    // 2. Check if task if successfully created
    if (status != pdPASS){
        abort();
    }

    // 3. Wait one second and then delete task
    vTaskDelay( pdMS_TO_TICKS(1000) );
    printf("Delete task_1\n");
    vTaskDelete(task_1);
    vTaskDelay( pdMS_TO_TICKS(1000) );

    // -----------------------------------
    // ----- Create Task Statically ------
    // -----------------------------------

    // 1. Create second task using static API
    printf("Create task_2\n");

    task_2 = xTaskCreateStatic(
                    task_2_function,  // called function
                    "task_2",         // human readable name
                    STACK_SIZE,       // stack size
                    NULL,             // task function parameter (void*)
                    1,                // task priority
                    task_2_stack,     // array for task stack
                    &task_2_TCB       // space for task TCB
    );

    // 2. Check if task if successfully created
    if (task_2 == NULL){
        abort();
    }

    // 3. Wait one second and then delete task
    vTaskDelay( pdMS_TO_TICKS(1000) );
    printf("Delete task_2\n");
    vTaskDelete(task_2);
}


void task_1_function(void* paramenter) {
    while(true){
        printf("Hello from task 1!\n");
        vTaskDelay( pdMS_TO_TICKS(100) );
    }
}


void task_2_function(void* paramenter) {
    while(true){
        printf("Hello from task 2!\n");
        vTaskDelay( pdMS_TO_TICKS(100) );
    }
}
