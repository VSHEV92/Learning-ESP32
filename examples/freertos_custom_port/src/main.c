#include <custom_init.h>
#include <FreeRTOS.h>
#include <task.h>

#define STACK_SIZE 1024

void task_1_function(void* paramenter);
void task_2_function(void* paramenter);

void __attribute__((noreturn)) main() {
    BaseType_t status;

    // run custom soc init
    custom_init();

    // reset task variables
    vTaskResetState();
    vPortHeapResetState();

    status = xTaskCreate(
                    task_1_function,  // called function
                    "task_1",         // human readable name
                    STACK_SIZE,       // stack size
                    NULL,             // task function parameter (void*)
                    1,                // task priority
                    NULL              // task handler
    );

    status = xTaskCreate(
                    task_2_function,  // called function
                    "task_2",         // human readable name
                    STACK_SIZE,       // stack size
                    NULL,             // task function parameter (void*)
                    1,                // task priority
                    NULL              // task handler
    );

    // start scheduler
    vTaskStartScheduler();
    
    for( ;; );
}

void task_1_function(void* paramenter) {
    while(true){
        ets_printf("Hello from task 1!\n");
        vTaskDelay( pdMS_TO_TICKS(500) );
    }
}

void task_2_function(void* paramenter) {
    while(true){
        ets_printf("Hello from task 2!\n");
        vTaskDelay( pdMS_TO_TICKS(700) );
    }
}
