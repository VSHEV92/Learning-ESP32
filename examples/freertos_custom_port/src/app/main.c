#include <custom_init.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <queue.h>

#define STACK_SIZE 1024
#define TIMER_PERIOD_MS 1000

static QueueHandle_t queue;

void send_function(void* paramenter);
void receive_function(void* paramenter);

void timer_callback( TimerHandle_t xTimer );


void __attribute__((noreturn)) main() {

    // ------------------------------------------------------------------
    // --------------------------- Start Up -----------------------------
    // ------------------------------------------------------------------
    // run custom soc init
    custom_init();

    // reset freertos component variables
    vTaskResetState();
    vPortHeapResetState();
    vTimerResetState();

    // ------------------------------------------------------------------
    // ------------ Queue, sender and receiver creation -----------------
    // ------------------------------------------------------------------
    queue = xQueueCreate( 
        1,                // number of items in queue 
        sizeof(uint32_t)  // size of one item in bytes
    );
    
    xTaskCreate(
        send_function,    // called function
        "sender",         // human readable name
        STACK_SIZE,       // stack size
        NULL,             // task function parameter (void*)
        1,                // task priority
        NULL              // task handler
    );

    xTaskCreate(
        receive_function,  // called function
        "receiver",        // human readable name
        STACK_SIZE,        // stack size
        NULL,              // task function parameter (void*)
        3,                 // task priority
        NULL               // task handler
    );

    // ------------------------------------------------------------------
    // ------------------- Timer creation and start ---------------------
    // ------------------------------------------------------------------
    TimerHandle_t autoreload_timer;
    autoreload_timer = xTimerCreate (
        "timer",                              // human readable timer name
        pdMS_TO_TICKS( TIMER_PERIOD_MS ),     // timer period in ticks
        pdTRUE,                               // enable autoreload
        ( void * ) 0,                         // set timer ID
        timer_callback                        // timer callback function 
    );

    xTimerStart(
        autoreload_timer,     // timer handler
        portMAX_DELAY         // max delay for waiting until command send to deamon command queue
    );
    
    // ------------------------------------------------------------------
    // ------------------------ Scheduler start -------------------------
    // ------------------------------------------------------------------
    vTaskStartScheduler();
    
    for( ;; );
}

void timer_callback( TimerHandle_t xTimer ) {
    int timer_id = ( int ) pvTimerGetTimerID( xTimer );
    ets_printf("Inside timer %d callback!\n", timer_id);
}

void send_function(void* paramenter) {
    uint32_t data_to_send = 0;
    
    while(true){
        ets_printf("Sender put %d value to queue!\n", (unsigned int)data_to_send);
        xQueueSendToBack(queue, (void*)&data_to_send, portMAX_DELAY);
        data_to_send += 10;
        vTaskDelay( pdMS_TO_TICKS(733) );
    }
}

void receive_function(void* paramenter) {
    uint32_t data_received;
    
    while(true){
        xQueueReceive(queue, &data_received, portMAX_DELAY);
        ets_printf("Receiver get %d value from queue!\n", (unsigned int)data_received);
    }
}
