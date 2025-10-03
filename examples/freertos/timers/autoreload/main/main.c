#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#define TIMER_NUMBER 5
#define TIMER_PERIOD_MS 1000

static const char* TAG = "freertos_example";

void timer_callback( TimerHandle_t xTimer );

void app_main(void) {
    TimerHandle_t autoreload_timer[TIMER_NUMBER];

    ESP_LOGI(TAG, "Create timers");

    for (int i = 0; i < TIMER_NUMBER; i++) {
        // create one shot timers
        autoreload_timer[i] = xTimerCreate (
            "one_shot",                                     // human readable timer name
            pdMS_TO_TICKS( TIMER_PERIOD_MS * (i + 1) ),     // timer period in ticks
            pdTRUE,                                         // enable autoreload
            ( void * ) i,                                   // set timer ID
            timer_callback                                  // timer callback function 
        );

        // check that timer successfully created
        if (autoreload_timer[i] == NULL) {
            ESP_LOGE(TAG, "Can't create timer %d!", i);
            abort();
        }
    }

    ESP_LOGI(TAG, "Start timers");

    for (int i = 0; i < TIMER_NUMBER; i++) {
        // start timer
        xTimerStart(
            autoreload_timer[i],  // timer handler
            portMAX_DELAY         // max delay for waiting until command send to deamon command queue
        );
    }

    // wait until all timers are triggered and stop timer 
    vTaskDelay( pdMS_TO_TICKS((TIMER_NUMBER + 1) * TIMER_PERIOD_MS) );

    ESP_LOGI(TAG, "Stop timers");

    for(int i = 0; i < TIMER_NUMBER; i++) {
        xTimerStop(
            autoreload_timer[i],  // timer handler
            portMAX_DELAY         // max delay for waiting until command send to deamon command queue
        );
    }

    // delete timers
    ESP_LOGI(TAG, "Delete timers");

    for(int i = 0; i < TIMER_NUMBER; i++) {
        xTimerDelete(
            autoreload_timer[i],  // timer handler
            portMAX_DELAY         // max delay for waiting until command send to deamon command queue
        );
    }
}

void timer_callback( TimerHandle_t xTimer ) {
    // get timer id
    int timer_id = ( int ) pvTimerGetTimerID( xTimer );

    // print timer message
    ESP_LOGI(TAG, "Hello from timer %d", timer_id);
}
