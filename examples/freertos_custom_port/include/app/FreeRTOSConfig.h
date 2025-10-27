
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configNUMBER_OF_CORES                  1

#define configCPU_CLOCK_HZ                     ( ( unsigned long ) 16000000 )
#define configTICK_RATE_HZ                     100

#define configUSE_PREEMPTION                   1
#define configUSE_TIME_SLICING                 1

#define configMAX_PRIORITIES                   5

#define configISR_STACK_SIZE_WORDS             1024
#define configMINIMAL_STACK_SIZE               4096

#define configTOTAL_HEAP_SIZE                  1024*128

#define configTICK_TYPE_WIDTH_IN_BITS          TICK_TYPE_WIDTH_32_BITS

#define configUSE_IDLE_HOOK                    0
#define configUSE_TICK_HOOK                    0

/* Timers configs*/
#define configUSE_TIMERS                       1
#define configTIMER_TASK_PRIORITY              configMAX_PRIORITIES
#define configTIMER_QUEUE_LENGTH               3
#define configTIMER_TASK_STACK_DEPTH           configMINIMAL_STACK_SIZE

/* Task functions */
#define INCLUDE_vTaskPrioritySet               1
#define INCLUDE_uxTaskPriorityGet              1
#define INCLUDE_vTaskDelete                    1
#define INCLUDE_vTaskSuspend                   1
#define INCLUDE_vTaskDelayUntil                1
#define INCLUDE_vTaskDelay                     1
#define INCLUDE_xTaskGetCurrentTaskHandle      1
#define INCLUDE_xTaskGetIdleTaskHandle         0
#define INCLUDE_xTimerPendFunctionCall         0
#define INCLUDE_xTaskGetHandle                 0
#define INCLUDE_xTaskResumeFromISR             1

#endif /* FREERTOS_CONFIG_H */
